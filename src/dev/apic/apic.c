#include "apic.h"

void register_apic(struct madt_header * madt, char* (*cb)(void*, uint8_t, uint64_t)) {
    (void)madt;
    (void)cb;
    return;
}