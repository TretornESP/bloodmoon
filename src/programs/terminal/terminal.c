#include <stdint.h>

#include "terminal.h"
#include "../../util/printf.h"
#include "../../util/string.h"
#include "../../sched/scheduler.h"
#include "../../drivers/gui/ux/ux.h"
#include "../../drivers/gui/gui.h"
#include "../../drivers/tty/tty.h"
#include "../../drivers/tty/tty_interface.h"
#include "../../drivers/fifo/fifo_interface.h"
#include "../../devices/devices.h"
#include "../../devices/comm/comm.h"
#include "../../debugger/dbgshell.h"
#include "../../memory/heap.h"

struct ux_component * terminal_root, * terminal_input, * terminal_output, * terminal_execute; 
char buffer[1024];
char *tname, *in_fifo, *out_fifo;
uint16_t bufsize = 0;
uint16_t pid = 0;
void (*tty_read_func)(void* ttyb, char c, int unused) = 0;

void execute_program() {
    //printf("Executing program: %s\n", buffer);

    ex_dbgshell(buffer);
    memset(buffer, 0, 1024);
    bufsize = 0;
}

void terminal_keypress_handler(struct ux_component * component, struct ux_event * event) {   
    if (event->data == 0) return;
    buffer[bufsize++] = (uint8_t)event->data;
    add_signal(pid, TERMINAL_SIGNAL_KEYPRESS, 0, 0);
}

void terminal_output_handler(struct ux_component * component, struct ux_event * event) {
    if (event->device != UX_EVENT_DEVICE_KEYBOARD) {
        return;
    }

    terminal_keypress_handler(component, event);
}

void terminal_signal_handler(int signo, void * ttyb, uint64_t event) {
    if (signo == TERMINAL_SIGNAL_TTY) {
        switch (event) {
            case 0x3: { //TTY_EVENT_FLUSH_OUTB
                char cmd[1024] = {0};
                int read = device_read(out_fifo, 1024, 0, (uint8_t*)cmd);
                if (read > 0) {
                    terminal_output->text[strlen(terminal_output->text)] = '\n';
                    memcpy(terminal_output->text + strlen(terminal_output->text), cmd, read);
                    terminal_output->requires_redraw = 1;
                }
            }
            default:
                break;
        }
    }

    if (signo == TERMINAL_SIGNAL_KEYPRESS) {
        memcpy(terminal_output->text + strlen(terminal_output->text), &buffer[bufsize - 1], 1);
        terminal_output->requires_redraw = 1;

        switch (buffer[bufsize - 1]) {
            case 8:
                //backspace, remove from buffer
                buffer[bufsize - 1] = 0;
                bufsize--;
                terminal_output->text[strlen(terminal_output->text) - 1] = 0;
                break;
            case 28:
                printf("\n");
                buffer[bufsize - 1] = 0;
                execute_program();
                printf("root@terminal:~$ ");
                break;
            default:
                break;
        }
    }

}

void terminal_signal_generator(void* ttyb, uint8_t event) {
    (void)ttyb;
    int pid = get_current_task()->pid;
    add_signal(pid, TERMINAL_SIGNAL_TTY, ttyb, (uint64_t)event);
}

void init_terminal() {
    if (!gui_is_ready()) {
        return 1;
    }

    pid = get_current_task()->pid;

    terminal_root = inflate_layout("Terminal", TERMINAL_LAYOUT);
    if (!terminal_root) {
        return 1;
    }
    terminal_output = get_component_by_name(terminal_root, "output");
    if (!terminal_output) {
        return 1;
    }

    hook_event(terminal_output, terminal_output_handler);

    in_fifo = create_fifo(1024);
    out_fifo = create_fifo(1024);
    tname = create_tty(in_fifo, out_fifo, TTY_MODE_PTY, 1024, 1024);

    subscribe_signal(TERMINAL_SIGNAL_TTY, terminal_signal_handler);
    subscribe_signal(TERMINAL_SIGNAL_KEYPRESS, terminal_signal_handler);
    device_ioctl(tname, 0x1, terminal_signal_generator); //ADD SUBSCRIBER    

    printf("Created terminal tty: %s\n", tname);
    set_current_tty(tname);
    printf("root@terminal:~$ ");

    while (1) {
        yield();
    }
}