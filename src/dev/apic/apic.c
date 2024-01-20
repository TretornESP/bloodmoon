#include "apic.h"
#include "../../util/printf.h"
#include "../../cpus/cpus.h"

void write_apic_register(uint32_t reg, uint32_t value) {
    uint32_t* apic = (uint32_t*)0xFEE00000;
    apic[reg/4] = value;
}

void parse_lapic_entry(struct lapic* lapic) {
    printf("[LAPIC] PROCID: %x, APICID: %x, FLAGS: %x\n", lapic->processor_id, lapic->apic_id, lapic->flags);
}

void parse_ioapic_entry(struct ioapic* ioapic) {
    printf("[IOAPIC] ID: %x, ADDR: %x, GSI: %x\n", ioapic->ioapic_id, ioapic->ioapic_address, ioapic->gsi_base);
}

void parse_ioapic_iso_entry(struct ioapic_iso* iso) {
    printf("[IOAPIC_ISO] BUS: %x, IRQ: %x, GSI: %x, FLAGS: %x\n", iso->bus_source, iso->irq_source, iso->gsi, iso->flags);
}

void parse_ioapic_nmi_entry(struct ioapic_nmi* nmi) {
    printf("[IOAPIC_NMI] NMI: %x, FLAGS: %x, GSI: %x\n", nmi->nmi_source, nmi->flags, nmi->gsi);
}

void parse_lapic_nmi_entry(struct lapic_nmi* nmi) {
    printf("[LAPIC_NMI] PROCID: %x, FLAGS: %x, LINT: %x\n", nmi->processor_id, nmi->flags, nmi->lint);
}

void parse_lapic_addr_override_entry(struct lapic_addr_override* override) {
    printf("[LAPIC_ADDR_OVERRIDE] ADDR: %x\n", override->address);
}

void parse_plx2apic_entry(struct plx2apic* entry) {
    printf("[PLX2APIC] PROCESSOR_LOCAL_X2APIC_ID: %x, FLAGS: %x, ACPI_ID: %x\n", entry->processor_local_x2apic_id, entry->flags, entry->acpi_id);
}

uint8_t parse_apic_entry(uint64_t* entry) {
    struct entry_record* ce = (struct entry_record*)*entry;
    printf("[%p] ", ce);

    switch(ce->type) {
        case MADT_RECORD_TYPE_LAPIC:
            printf("LAPIC: %x\n", ce->length);
            parse_lapic_entry((struct lapic*)ce);
            break;
        case MADT_RECORD_TYPE_IOAPIC:
            printf("IOAPIC: %x\n", ce->length);
            parse_ioapic_entry((struct ioapic*)ce);
            break;

        case MADT_RECORD_TYPE_IOAPIC_ISO:
            printf("IOAPIC_ISO: %x\n", ce->length);
            parse_ioapic_iso_entry((struct ioapic_iso*)ce);
            break;

        case MADT_RECORD_TYPE_IOAPIC_NMI:
            printf("IOAPIC_NMI: %x\n", ce->length);
            parse_ioapic_nmi_entry((struct ioapic_nmi*)ce);
            break;

        case MADT_RECORD_TYPE_LAPIC_NMI:
            printf("LAPIC_NMI: %x\n", ce->length);
            parse_lapic_nmi_entry((struct lapic_nmi*)ce);
            break;

        case MADT_RECORD_TYPE_LAPIC_ADDR_OVERRIDE:
            printf("LAPIC_ADDR_OVERRIDE: %x\n", ce->length);
            parse_lapic_addr_override_entry((struct lapic_addr_override*)ce);
            break;

        case MADT_RECORD_TYPE_PLX2APIC:
            printf("PLX2APIC: %x\n", ce->length);
            parse_plx2apic_entry((struct plx2apic*)ce);
            break;

        default:
            printf("Unknown APIC entry type: %x len: %x\n", ce->type, ce->length);
            break;
    }

    uint8_t length = ce->length;
    *entry += length;
    return length;
}

void register_apic(struct madt_header * madt, char* (*cb)(void*, uint8_t, uint64_t)) {
    (void)cb;
    printf("APIC: %x\n", madt->local_apic_address);
    printf("flags: %x\n", madt->flags);
    printf("length: %x\n", madt->header.length);

    uint64_t entry = (((uint64_t)madt)+sizeof(struct madt_header));
    uint64_t end = ((uint64_t)madt)+madt->header.length;
    while(entry < end) {
        uint8_t len = parse_apic_entry(&entry);
        if(len == 0) {
            break;
        }
    }

    while(1);
}