#include <stdint.h>
#include "ps2.h"
#include "mouse.h"
#include "keyboard.h"
#include "ps2_dd.h"
#include "../../io/io.h"
#include "../../io/interrupts.h"
#include "../../cpus/cpus.h"

struct ps2_mouse_status mouse;

void KeyboardInt_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    uint8_t scancode = inb(0x60);
    handle_keyboard(scancode);
}

void MouseInt_Handler(struct cpu_context* ctx, uint8_t cpuid) {
    (void)ctx;
    (void)cpuid;
    uint8_t scancode = inb(0x60);
    handle_mouse(scancode);
}

struct ps2_mouse_status * read_mouse() {
    if (process_mouse_packet(&mouse)) {
        return &mouse;
    } else {
        return 0;
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