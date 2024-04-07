#include <stdint.h>
#include "ps2.h"
#include "mouse.h"
#include "keyboard.h"
#include "ps2_dd.h"
#include "../../io/io.h"
#include "../../io/interrupts.h"
#include "../../cpus/cpus.h"
#include "../../memory/heap.h"

struct ps2_subscriber {
    void (*callback)(void*);
    struct ps2_subscriber *next;
};

struct ps2_subscriber *keyboard_all_subscribers = 0;
struct ps2_subscriber *mouse_all_subscribers = 0;
struct ps2_subscriber *keyboard_event_subscribers = 0;
struct ps2_subscriber *mouse_event_subscribers = 0;

void KeyboardInt_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    uint8_t scancode = inb(0x60);
    handle_keyboard(scancode);
}

void MouseInt_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    struct ps2_mouse_status mouse;
    uint8_t scancode = inb(0x60);
    handle_mouse(scancode);

    if (process_current_mouse_packet(&mouse)) {
        struct ps2_subscriber *current = mouse_all_subscribers;
        while (current) {
            current->callback(&mouse);
            current = current->next;
        }
        if (mouse.buttons) {
            current = mouse_event_subscribers;
            while (current) {
                current->callback(&mouse);
                current = current->next;
            }
        }
    }

}

uint8_t read_mouse(struct ps2_mouse_status * mouse) {
    if (get_mouse(mouse)) {
        return 1;
    } else {
        return 0;
    }
}

void _ps2_subscribe(struct ps2_subscriber **list, void (*callback)(void*)) {
    struct ps2_subscriber *new_subscriber = (struct ps2_subscriber*)malloc(sizeof(struct ps2_subscriber));
    new_subscriber->callback = callback;
    new_subscriber->next = *list;
    *list = new_subscriber;
}

void ps2_subscribe(void (*callback)(void*), uint8_t device, uint8_t event) {
    if (device == PS2_DEVICE_MOUSE) {
        if (event == PS2_DEVICE_EVENT) {
            _ps2_subscribe(&mouse_event_subscribers, callback);
        } else if (event == PS2_DEVICE_ALL) {
            _ps2_subscribe(&mouse_all_subscribers, callback);
        } else {
            return;
        }
    } else if (device == PS2_DEVICE_KEYBOARD) {
        if (event == PS2_DEVICE_EVENT) {
            _ps2_subscribe(&keyboard_event_subscribers, callback);
        } else if (event == PS2_DEVICE_ALL) {
            _ps2_subscribe(&keyboard_all_subscribers, callback);
        } else {
            return;
        }
    }
    
    return;
}

void ps2_unsubscribe(void (*callback)(void*)) {
    struct ps2_subscriber *current = keyboard_all_subscribers;
    struct ps2_subscriber *prev = 0;
    while (current) {
        if (current->callback == callback) {
            if (prev) {
                prev->next = current->next;
            } else {
                keyboard_all_subscribers = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }

    current = mouse_all_subscribers;
    prev = 0;
    while (current) {
        if (current->callback == callback) {
            if (prev) {
                prev->next = current->next;
            } else {
                mouse_all_subscribers = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }

    current = keyboard_event_subscribers;
    prev = 0;
    while (current) {
        if (current->callback == callback) {
            if (prev) {
                prev->next = current->next;
            } else {
                keyboard_event_subscribers = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }

    current = mouse_event_subscribers;
    prev = 0;
    while (current) {
        if (current->callback == callback) {
            if (prev) {
                prev->next = current->next;
            } else {
                mouse_event_subscribers = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void init_ps2() {
    init_keyboard();
    init_mouse();

    hook_interrupt(KBD_IRQ, (void*)KeyboardInt_Handler);
    hook_interrupt(MSE_IRQ, (void*)MouseInt_Handler);
    unmask_interrupt(KBD_IRQ);
    unmask_interrupt(MSE_IRQ);

    init_mouse_dd();
}