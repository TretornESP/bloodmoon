#include <stdint.h>
#include "ps2.h"
#include "mouse.h"
#include "keyboard.h"
#include "ps2_dd.h"
#include "../../io/io.h"
#include "../../io/interrupts.h"
#include "../../cpus/cpus.h"
#include "../../util/printf.h"
#include "../../memory/heap.h"

struct ps2_subscriber {
    void * parent;
    void (*handler)(void* data, char c, int ignore);
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
    char result = handle_keyboard(scancode);

    struct ps2_subscriber *current = keyboard_all_subscribers;
    while (current) {
        current->handler(current->parent, result, 0);
        current = current->next;
    }
    current = keyboard_event_subscribers;
    while (current) {
        current->handler(current->parent, result, 0);
        current = current->next;
    }
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
            if (current->handler)
                current->handler((void*)&mouse, 0, 0);
            else
                printf("Handler is null for sub at %p\n", current);
            current = current->next;
        }
        if (mouse.buttons) {
            current = mouse_event_subscribers;
            while (current) {
                if (current->handler)
                    current->handler((void*)&mouse, 0, 0);
                else
                    printf("Evt handler is null for sub at %p\n", current);
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

uint8_t read_keyboard(char * key) {
    *key = get_last_key();
    return 1;
}

void keyboard_halt_until_enter() {
    halt_until_enter();
}

void ps2_subscribe(void* handler, uint8_t device, uint8_t event) {
    struct ps2_subscriber *new_subscriber = (struct ps2_subscriber*)kmalloc(sizeof(struct ps2_subscriber));
    struct ps2_subscriber *current = 0;
    if (device == PS2_DEVICE_MOUSE) {
        new_subscriber->parent = 0x0;
        new_subscriber->handler = handler;
        
        if (event == PS2_DEVICE_GENERIC_EVENT) {
            current = mouse_all_subscribers;
            new_subscriber->next = current;
            mouse_all_subscribers = new_subscriber;
        } else if (event == PS2_DEVICE_SPECIAL_EVENT) {
            current = mouse_event_subscribers;
            new_subscriber->next = current;
            mouse_event_subscribers = new_subscriber;
        } else {
            return;
        }
        
    } else if (device == PS2_DEVICE_KEYBOARD) {
        struct ps2_kbd_ioctl_subscriptor *kbdsub = (struct ps2_kbd_ioctl_subscriptor*)handler;
        new_subscriber->parent = kbdsub->parent;
        new_subscriber->handler = kbdsub->handler;

        if (event == PS2_DEVICE_GENERIC_EVENT) {
            current = keyboard_all_subscribers;
            new_subscriber->next = current;
            keyboard_all_subscribers = new_subscriber;
        } else {
            return;
        }
    }
}

void ps2_unsubscribe(void *handler) {
    struct ps2_subscriber *current = keyboard_all_subscribers;
    struct ps2_subscriber *prev = 0;
    while (current) {
        if (current->handler == handler) {
            if (prev) {
                prev->next = current->next;
            } else {
                keyboard_all_subscribers = current->next;
            }
            kfree(current);
            return;
        }
        prev = current;
        current = current->next;
    }

    current = mouse_all_subscribers;
    prev = 0;
    while (current) {
        if (current->handler == handler) {
            if (prev) {
                prev->next = current->next;
            } else {
                mouse_all_subscribers = current->next;
            }
            kfree(current);
            return;
        }
        prev = current;
        current = current->next;
    }

    current = keyboard_event_subscribers;
    prev = 0;
    while (current) {
        if (current->handler == handler) {
            if (prev) {
                prev->next = current->next;
            } else {
                keyboard_event_subscribers = current->next;
            }
            kfree(current);
            return;
        }
        prev = current;
        current = current->next;
    }

    current = mouse_event_subscribers;
    prev = 0;
    while (current) {
        if (current->handler == handler) {
            if (prev) {
                prev->next = current->next;
            } else {
                mouse_event_subscribers = current->next;
            }
            kfree(current);
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
    init_keyboard_dd();
}