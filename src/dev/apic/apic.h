#ifndef _APIC_H
#define _APIC_H
//https://wiki.osdev.org/MADT
#include "../acpi/acpi.h"
#include <stdint.h>

#define MADT_RECORD_TYPE_LAPIC                  0
#define MADT_RECORD_TYPE_IOAPIC                 1
#define MADT_RECORD_TYPE_IOAPIC_ISO             2
#define MADT_RECORD_TYPE_IOAPIC_NMI             3
#define MADT_RECORD_TYPE_LAPIC_NMI              4
#define MADT_RECORD_TYPE_LAPIC_ADDR_OVERRIDE    5
#define MADT_RECORD_TYPE_PLX2APIC               9

#define MADT_LAPIC_FLAGS_ENABLED                1

#define MADT_IOAPIC_ISO_FLAGS_ACTIVE_LOW        0x2
#define MADT_IOAPIC_ISO_FLAGS_LEVEL_TRIGGERED   0x8        



struct entry_record {
    uint8_t type;
    uint8_t length;
} __attribute__((packed));

struct lapic {
    struct entry_record record;
    uint8_t processor_id;
    uint8_t apic_id;
    uint32_t flags; //bit 0: processor enabled, bit 1: online capable
} __attribute__((packed));

struct ioapic {
    struct entry_record record;
    uint8_t ioapic_id;
    uint8_t reserved;
    uint32_t ioapic_address;
    uint32_t gsi_base;
} __attribute__((packed));

struct ioapic_iso {
    struct entry_record record;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t gsi;
    uint16_t flags;
} __attribute__((packed));

struct ioapic_nmi {
    struct entry_record record;
    uint8_t nmi_source;
    uint8_t reserved;
    uint16_t flags;
    uint32_t gsi;
} __attribute__((packed));

struct lapic_nmi {
    struct entry_record record;
    uint8_t processor_id; //0xFF for all
    uint16_t flags;
    uint8_t lint; //0 or 1
} __attribute__((packed));

struct lapic_addr_override {
    struct entry_record record;
    uint16_t reserved;
    uint64_t address;
} __attribute__((packed));

struct plx2apic {
    struct entry_record record;
    uint16_t reserved;
    uint32_t processor_local_x2apic_id;
    uint32_t flags;
    uint32_t acpi_id;
} __attribute__((packed));

void register_apic(struct madt_header *, char* (*cb)(void*, uint8_t, uint64_t));

#endif