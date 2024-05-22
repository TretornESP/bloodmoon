#include "gdt.h"
#include "tss.h"
#include "../util/string.h"
#include "../util/printf.h"

static __attribute__((aligned(0x1000))) gdt_t gdt[GDT_MAX_CPU];
static struct tss _tss = {
    .reserved0 = 0,
    .rsp = {},
    .reserved1 = 0,
    .ist = {},
    .reserved2 = 0,
    .reserved3 = 0,
    .iopb = 0,
};

gdt_entry_t gdt_entry(uint32_t base, uint32_t limit, uint8_t granularity, uint8_t flags) {
    return (gdt_entry_t) {
        .limit_low = (uint16_t)((limit)&0xffff),
        .base_low = (uint16_t)((base)&0xffff),
        .base_middle = (uint8_t)(((base) >> 16) & 0xff),
        .flags = (flags),
        .limit_middle = ((limit) >> 16) & 0x0f,
        .granularity = (granularity),
        .base_high = (uint8_t)(((base) >> 24) & 0xff),
    };
}

gdt_entry_t gdt_null_entry(void) {
    return gdt_entry(0, 0, 0, 0);
}

gdt_entry_t gdt_simple_entry(uint8_t flags, uint8_t granularity) {
    return gdt_entry(0, 0, granularity, flags);
}

tss_entry_t gdt_tss_entry(uintptr_t tss_address) {
    return (tss_entry_t) {
        .length = sizeof(struct tss),
        .base_low = tss_address & 0xffff,
        .base_middle = (tss_address >> 16) & 0xff,
        .flags1 = 0b10001001,
        .flags2 = 0,
        .base_high = (tss_address >> 24) & 0xff,
        .base_upper = tss_address >> 32,
        .reserved = 0,
    };
}

void dump_gdt_entry(gdt_entry_t* entry) {
    printf("  GDT Entry: 0x%08x\n", *(uint32_t*) entry);
    printf("  Base: 0x%08x\n", entry->base_low | (entry->base_middle << 16) | (entry->base_high << 24));
    printf("  Limit: 0x%08x\n", entry->limit_low | (entry->limit_middle << 16));
    printf("  Flags: 0x%02x\n", entry->flags);
    printf("  Granularity: 0x%02x\n", entry->granularity);
}

void dump_tss_entry(tss_entry_t* entry) {
    printf("  TSS Entry: 0x%08x\n", *(uint32_t*) entry);
    printf("  Base: 0x%08x\n", entry->base_low | (entry->base_middle << 16) | (entry->base_high << 24));
    printf("  Limit: 0x%08x\n", entry->length);
    printf("  Flags1: 0x%02x\n", entry->flags1);
    printf("  Flags2: 0x%02x\n", entry->flags2);
    printf("  Base Upper: 0x%08x\n", entry->base_upper);
    printf("  Reserved: 0x%08x\n", entry->reserved);
}

void dump_gdt(gdt_t* gdt) {
    printf("Null Entry:\n");
    dump_gdt_entry(&gdt->entries[GDT_NULL_ENTRY]);
    printf("Kernel Code Entry:\n");
    dump_gdt_entry(&gdt->entries[GDT_KERNEL_CODE_ENTRY]);
    printf("Kernel Data Entry:\n");
    dump_gdt_entry(&gdt->entries[GDT_KERNEL_DATA_ENTRY]);
    printf("Null Entry 2:\n");
    dump_gdt_entry(&gdt->entries[GDT_NULL_ENTRY_2]);
    printf("User Data Entry:\n");
    dump_gdt_entry(&gdt->entries[GDT_USER_DATA_ENTRY]);
    printf("User Code Entry:\n");
    dump_gdt_entry(&gdt->entries[GDT_USER_CODE_ENTRY]);
    printf("TSS Entry:\n");
    dump_tss_entry(&gdt->tss);
}

void create_gdt() {
    memset(&_tss, 0, sizeof(struct tss));
    _tss.iopb = 0;

    for (uint8_t i = 0; i < GDT_MAX_CPU; i++) {
        gdt_t* cgdt = &gdt[i];
        memset(cgdt, 0, sizeof(gdt_t));

        cgdt->entries[GDT_NULL_ENTRY] = gdt_null_entry(); //0 
        cgdt->entries[GDT_KERNEL_CODE_ENTRY] = gdt_simple_entry(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_EXECUTABLE, GDT_LONG_MODE_GRANULARITY); //8
        cgdt->entries[GDT_KERNEL_DATA_ENTRY] = gdt_simple_entry(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE, 0); //16
        cgdt->entries[GDT_NULL_ENTRY_2] = gdt_null_entry(); //24
        cgdt->entries[GDT_USER_DATA_ENTRY] = gdt_simple_entry(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_USER, 0); //32
        cgdt->entries[GDT_USER_CODE_ENTRY] = gdt_simple_entry(GDT_PRESENT | GDT_SEGMENT | GDT_READWRITE | GDT_EXECUTABLE | GDT_USER, GDT_LONG_MODE_GRANULARITY); //40
        cgdt->tss = gdt_tss_entry((uintptr_t) &_tss); //48
    }

    printf("Dumping GDT for CPU 0\n");
    dump_gdt(&gdt[0]);
}

void load_gdt(uint8_t cpu) {
    _load_gdt(&(struct gdt_descriptor) {
        .size = sizeof(gdt_t) - 1,
        .offset = (uint64_t) &gdt[cpu],
    });
    __asm__("movw %%ax, %w0\n\t" "ltr %%ax" :: "a" (GDT_TSS_ENTRY << 3));
}

struct tss *get_tss(uint64_t index) {
    return &_tss;
}

uint16_t get_kernel_code_selector() {
    return GDT_KERNEL_CODE_ENTRY << 3;
}
uint16_t get_kernel_data_selector() {
    return GDT_KERNEL_DATA_ENTRY << 3;
}
uint16_t get_user_code_selector() {
    return GDT_USER_CODE_ENTRY << 3;
}
uint16_t get_user_data_selector() {
    return GDT_USER_DATA_ENTRY << 3;
}
uint16_t get_tss_selector() {
    return GDT_TSS_ENTRY << 3;
}