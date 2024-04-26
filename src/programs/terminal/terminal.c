#include <stdint.h>

#include "terminal.h"
#include "../../util/printf.h"
#include "../../util/string.h"
#include "../../scheduling/scheduler.h"
#include "../../drivers/gui/ux/ux.h"
#include "../../drivers/gui/gui.h"

struct ux_component * terminal_root, * terminal_input, * terminal_output, * terminal_execute; 
char buffer[1024];
uint8_t process_requested = 0;

void execute_program(char * buffer) {
    printf("Executing program: %s\n", buffer);
    terminal_output->text[strlen(terminal_output->text)] = '\n';
    memcpy(terminal_output->text + strlen(terminal_output->text), buffer, strlen(buffer));
    redraw_component(terminal_output);
}

void terminal_keypress_handler(struct ux_component * component, struct ux_event * event) {
    if (process_requested) {
        return;
    }

    buffer[strlen(buffer)] = event->data;
    printf("Key pressed: %c\n", event->data);
    component->text[strlen(component->text)] = event->data;
    redraw_component(component);
}

void terminal_buttonpress_handler(struct ux_component * component, struct ux_event * event) {
    process_requested = 1;
}

void init_terminal() {
    if (!gui_is_ready()) {
        return 1;
    }

    terminal_root = inflate_layout("Terminal", TERMINAL_LAYOUT);
    if (!terminal_root) {
        return 1;
    }
    terminal_input = get_component_by_name(terminal_root, "input");
    if (!terminal_input) {
        return 1;
    }
    terminal_output = get_component_by_name(terminal_root, "output");
    if (!terminal_output) {
        return 1;
    }
    terminal_execute = get_component_by_name(terminal_root, "execute");
    if (!terminal_execute) {
        return 1;
    }

    hook_event(terminal_input, terminal_keypress_handler);
    hook_event(terminal_execute, terminal_buttonpress_handler);
    redraw_component(terminal_root);

    while (1) {
        if (process_requested) {
            process_requested = 0;
            char buf2[1024];
            strcpy(buf2, buffer);
            memset(buffer, 0, 1024);
            execute_program(buffer);
            process_requested = 0;
        }
    }
}