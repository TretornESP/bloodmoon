#include "ints.h"

void activate_interrupts() {
    atomic_store(&ints_activated, 1);
}
void deactivate_interrupts() {
    __asm__("cli");
    atomic_store(&ints_activated, 0);
}

int interrupts_activated() {
    return atomic_load(&ints_activated);
}

void sti() {
    if (atomic_load(&ints_activated)){
        atomic_store(&ints_enabled, 1);
        __asm__("sti");
    } else {
        panic("Interrupts not ready");
    }
}

void cli() {
    __asm__("cli");
    atomic_store(&ints_enabled, 0);
}