#include "fadt.h"
#include "../apic/apic.h"
#include "../../io/io.h"
#include "../../util/printf.h"

uint64_t reset_reg;
uint8_t reset_value;

void reboot() {
    outb(reset_reg, reset_value);
}

void dump_fadt(struct fadt_header* fadt) {
    (void)fadt;
    printf("FADT:\n");
    return;
}

void register_fadt(struct fadt_header* fadt, char* (*cb)(void*, uint8_t, uint64_t)) {
    (void)cb;
    dump_fadt(fadt);
    reset_reg = fadt->reset_reg.address;
    reset_value = fadt->reset_value;
    return;
}