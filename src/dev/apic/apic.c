#include "apic.h"
#include "../../util/printf.h"

void register_apic(struct madt_header * madt, char* (*cb)(void*, uint8_t, uint64_t)) {
    (void)cb;
    printf("APIC: %x\n", madt->local_apic_address);
    while(1);
}