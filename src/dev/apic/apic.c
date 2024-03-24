//This code is based on Kot's implementation https://github.com/kot-org/Kot/blob/main/sources/core/kernel/source/arch/amd64/apic.c
#include "apic.h"
#include "../../util/printf.h"
#include "../../memory/heap.h"
#include "../../memory/paging.h"
#include "../../cpus/cpus.h"
#include "../../arch/msr.h"
#include "../../arch/capabilities.h"
#include "../../io/io.h"
#include "../../io/interrupts.h"

struct apic_context actx;

void write_lapic_register(void * lapic_addr, uint64_t offset, uint32_t value) {
    *((volatile uint32_t*)((void*)((uint64_t)lapic_addr+offset))) = value;
}

uint32_t read_lapic_register(void * lapic_addr, uint64_t offset) {
    return *((volatile uint32_t*)((void*)((uint64_t)lapic_addr+offset)));
}

uint32_t ioapic_read_register(void* apic_ptr, uint8_t offset) {
    *(volatile uint32_t*)(apic_ptr) = offset;
    return *(volatile uint32_t*)((uint64_t)apic_ptr+0x10);
}

void ioapic_write_register(void* apic_ptr, uint8_t offset, uint32_t value) {
    *(volatile uint32_t*)(apic_ptr) = offset;
    *(volatile uint32_t*)((uint64_t)apic_ptr+0x10) = value;
}

void ioapic_set_redirection_entry(void* apic_ptr, uint64_t index, struct ioapic_redirection_entry entry) {
    volatile uint32_t low = (
        (entry.vector << IOAPIC_REDIRECTION_BITS_VECTOR) |
        (entry.delivery_mode << IOAPIC_REDIRECTION_BITS_DELIVERY_MODE) |
        (entry.destination_mode << IOAPIC_REDIRECTION_BITS_DESTINATION_MODE) |
        (entry.delivery_status << IOAPIC_REDIRECTION_BITS_DELIVERY_STATUS) |
        (entry.pin_polarity << IOAPIC_REDIRECTION_BITS_PIN_POLARITY) |
        (entry.remote_irr << IOAPIC_REDIRECTION_BITS_REMOTE_IRR) |
        (entry.trigger_mode << IOAPIC_REDIRECTION_BITS_TRIGGER_MODE) |
        (entry.mask << IOAPIC_REDIRECTION_BITS_MASK)
    );

    volatile uint32_t high = (
        (entry.destination << IOAPIC_REDIRECTION_BITS_DESTINATION)
    );

    ioapic_write_register(apic_ptr, IOAPIC_REDIRECTION_TABLE + (index * 2), low);
    ioapic_write_register(apic_ptr, IOAPIC_REDIRECTION_TABLE + (index * 2) + 1, high);
}

uint8_t account_apic_entry(uint64_t* entry) {
    struct entry_record* ce = (struct entry_record*)*entry;

    switch(ce->type) {
        case MADT_RECORD_TYPE_LAPIC:
            struct lapic* lapic = (struct lapic*)ce;
            if (lapic->apic_id > actx.max_apic_id) {
                actx.max_apic_id = lapic->apic_id;
            }
            actx.lapic_count++;
            break;
        case MADT_RECORD_TYPE_IOAPIC:
            actx.ioapic_count++;
            break;
        case MADT_RECORD_TYPE_IOAPIC_ISO:
            actx.ioapic_iso_count++;
            break;
        case MADT_RECORD_TYPE_IOAPIC_NMI:
            break;
        case MADT_RECORD_TYPE_LAPIC_NMI:
            break;
        case MADT_RECORD_TYPE_LAPIC_ADDR_OVERRIDE:
            break;
        case MADT_RECORD_TYPE_PLX2APIC:
            break;
        default:
            break;
    }

    uint8_t length = ce->length;
    *entry += length;
    return length;
}

uint8_t parse_apic_entry(uint64_t* entry) {
    struct entry_record* ce = (struct entry_record*)*entry;

    static uint8_t lapic_index = 0;
    static uint8_t ioapic_index = 0;
    static uint8_t ioapic_iso_index = 0;

    switch(ce->type) {
        case MADT_RECORD_TYPE_LAPIC:
            struct lapic* lapic = (struct lapic*)ce;
            actx.lapics[lapic_index++] = lapic;
            actx.lapic_address[lapic->apic_id] = (struct lapic_address*)malloc(sizeof(struct lapic_address));
            break;
        case MADT_RECORD_TYPE_IOAPIC:
            actx.ioapics[ioapic_index++] = (struct ioapic*)ce;
            break;
        case MADT_RECORD_TYPE_IOAPIC_ISO:
            actx.ioapic_isos[ioapic_iso_index++] = (struct ioapic_iso*)ce;
            break;
        case MADT_RECORD_TYPE_IOAPIC_NMI:
            break;
        case MADT_RECORD_TYPE_LAPIC_NMI:
            break;
        case MADT_RECORD_TYPE_LAPIC_ADDR_OVERRIDE:
            break;
        case MADT_RECORD_TYPE_PLX2APIC:
            break;
        default:
            break;
    }

    uint8_t length = ce->length;
    *entry += length;
    return length;
}

void* get_lapic_address() {
    return actx.lapic_address[getApicId()]->virtual_address;
}

void enable_apic(uint8_t cpu_id) {
    uint32_t lo;
    uint32_t hi;
    cpuGetMSR(0x1B, &lo, &hi);

    uint64_t base_addr = (((uint64_t)hi << 32) | lo) & 0xFFFFF000;
    map_current_memory((void*)base_addr, (void*)base_addr);

    actx.lapic_address[cpu_id]->physical_address = (void*)base_addr;
    actx.lapic_address[cpu_id]->virtual_address = (void*)base_addr;

    void * lapic_address = get_lapic_address();
º
    uint64_t current_local_destination = read_lapic_register(lapic_address, LAPIC_LOGICAL_DESTINATION);
    uint64_t current_svr = read_lapic_register(lapic_address, LAPIC_SPURIOUS_INTERRUPT_VECTOR);

    write_lapic_register(lapic_address, LAPIC_DESTINATION_FORMAT, 0xffffffff);
    write_lapic_register(lapic_address, LAPIC_LOGICAL_DESTINATION, ((current_local_destination & ~((0xff << 24))) | (cpu_id << 24)));
    write_lapic_register(lapic_address, LAPIC_SPURIOUS_INTERRUPT_VECTOR, current_svr | LOCAL_APIC_SPURIOUS_ALL | LOCAL_APIC_SPURIOUS_ENABLE_APIC);
    write_lapic_register(lapic_address, LAPIC_TASK_PRIORITY, 0);

    uint64_t value = ((uint64_t)actx.lapic_address[cpu_id]->physical_address) | (LOCAL_APIC_ENABLE & ~((1 << 10)));
    lo = value & 0xFFFFFFFF;
    hi = value >> 32;
    cpuSetMSR(0x1B, lo, hi);
}

void ioapic_init(uint64_t ioapic_id) {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);

    enable_apic(getApicId());

    struct ioapic* ioapic = actx.ioapics[ioapic_id];
    uint64_t ioapic_address = (uint64_t)ioapic->ioapic_address;
    map_current_memory((void*)ioapic_address, (void*)ioapic_address);
    uint64_t ioapic_version = ioapic_read_register(
        (void*)ioapic_address,
        IOAPIC_VERSION
    );

    uint8_t max_interrupts = ((ioapic_version >> 16) & 0xff) + 1;
    ioapic->max_interrupts = max_interrupts;

    uint32_t base = ioapic->gsi_base;

    for (uint64_t i = 0; i < max_interrupts; i++) {
        struct ioapic_redirection_entry entry = {
            .vector = IRQ_START + i,
            .delivery_mode = IOAPIC_REDIRECTION_DELIVERY_MODE_FIXED,
            .destination_mode = IOAPIC_REDIRECTION_DESTINATION_MODE_PHYSICAL,
            .delivery_status = IOAPIC_REDIRECTION_ENTRY_DELIVERY_STATUS_IDLE,
            .pin_polarity = IOAPIC_REDIRECTION_PIN_POLARITY_ACTIVE_HIGH,
            .remote_irr = IOAPIC_REDIRECTION_REMOTE_IRR_NONE,
            .trigger_mode = IOAPIC_REDIRECTION_TRIGGER_MODE_EDGE,
            .mask = IOAPIC_REDIRECTION_MASK_DISABLE,
            .destination = 0
        };

        ioapic_set_redirection_entry((void*)ioapic_address, i - base, entry);
    }

    for (uint64_t i = 0; i < actx.ioapic_iso_count; i++) {
        struct ioapic_iso* iso = actx.ioapic_isos[i];
        uint8_t irq_number = iso->irq_source + IRQ_START;
        struct ioapic_redirection_entry entry = {
            .vector = irq_number,
            .delivery_mode = IOAPIC_REDIRECTION_DELIVERY_MODE_FIXED,
            .destination_mode = IOAPIC_REDIRECTION_DESTINATION_MODE_PHYSICAL,
            .delivery_status = IOAPIC_REDIRECTION_ENTRY_DELIVERY_STATUS_IDLE,
            .pin_polarity = (iso->flags & 0x3) == 0x03 ? IOAPIC_REDIRECTION_PIN_POLARITY_ACTIVE_LOW : IOAPIC_REDIRECTION_PIN_POLARITY_ACTIVE_HIGH,
            .remote_irr = IOAPIC_REDIRECTION_REMOTE_IRR_NONE,
            .trigger_mode = (iso->flags & 0xc) == 0x0c ? IOAPIC_REDIRECTION_TRIGGER_MODE_LEVEL : IOAPIC_REDIRECTION_TRIGGER_MODE_EDGE,
            .mask = IOAPIC_REDIRECTION_MASK_DISABLE,
            .destination = 0
        };

        ioapic_set_redirection_entry((void*)ioapic_address, iso->irq_source, entry);
    }
}

void register_apic(struct madt_header * madt, char* (*cb)(void*, uint8_t, uint64_t)) {
    (void)cb;

    actx.lapic_count = 0;
    actx.ioapic_count = 0;
    actx.ioapic_iso_count = 0;
    actx.max_apic_id = 0;

    printf("APIC: %x\n", madt->local_apic_address);
    printf("flags: %x\n", madt->flags);
    printf("length: %x\n", madt->header.length);

    uint64_t entry = (((uint64_t)madt)+sizeof(struct madt_header));
    uint64_t end = ((uint64_t)madt)+madt->header.length;
    while(entry < end) {
        uint8_t len = account_apic_entry(&entry);
        if(len == 0) {
            break;
        }
    }
    actx.lapics = (struct lapic**)malloc(sizeof(struct lapic)*actx.lapic_count);
    actx.lapic_address = (struct lapic_address**)malloc(sizeof(struct lapic_address*)* (actx.max_apic_id+1));
    actx.ioapics = (struct ioapic**)malloc(sizeof(struct ioapic)*actx.ioapic_count);
    actx.ioapic_isos = (struct ioapic_iso**)malloc(sizeof(struct ioapic_iso)*actx.ioapic_iso_count);

    entry = (((uint64_t)madt)+sizeof(struct madt_header));
    end = ((uint64_t)madt)+madt->header.length;
    while(entry < end) {
        uint8_t len = parse_apic_entry(&entry);
        if(len == 0) {
            break;
        }
    }

    for (uint64_t i = 0; actx.ioapic_count; i++) {
        ioapic_init(i);
    }

}