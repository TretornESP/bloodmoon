#include "gdt.h"
#include "tss.h"
#include "../memory/paging.h"
#include "../memory/heap.h"
#include "../util/string.h"
#include "../util/printf.h"

static __attribute__((aligned(0x1000))) struct gdt_entry gdt[GDT_MAX_CPU][MAX_GDT_ENTRIES];
static __attribute__((aligned(0x1000))) struct gdt_descriptor descriptor[GDT_MAX_CPU];
static __attribute__((aligned(0x1000))) struct tss tss[GDT_MAX_CPU];
int size[GDT_MAX_CPU];

extern void _load_gdt(struct gdt_descriptor *gdt);

uint16_t create_code_descriptor(uint8_t cpuid, uint8_t dpl) {
    struct gdt_entry *entry = &gdt[cpuid][size[cpuid]++];
    entry->limit0 = 0;
    entry->base0 = 0;
    entry->base1 = 0;

    struct gdt_access_byte *access = (struct gdt_access_byte *) &entry->access;
    access->a = 0;
    access->rw = 1;
    access->dc = 0;
    access->ex = 1;
    access->s = 1;
    access->dpl = dpl;
    access->p = 1;

    struct gdt_flags * flags = (struct gdt_flags *) &entry->flags_and_limit1;
    flags->limit = 0;
    flags->reserved = 0;
    flags->l = 1;
    flags->db = 0;
    flags->g = 1;

    entry->base2 = 0;
    return (size[cpuid] - 1) * sizeof(struct gdt_entry);
}

uint16_t create_data_descriptor(uint8_t cpuid, uint8_t dpl) {
    struct gdt_entry *entry = &gdt[cpuid][size[cpuid]++];
    entry->limit0 = 0;
    entry->base0 = 0;
    entry->base1 = 0;

    struct gdt_access_byte *access = (struct gdt_access_byte *) &entry->access;
    access->a = 0;
    access->rw = 1;
    access->dc = 0;
    access->ex = 0;
    access->s = 1;
    access->dpl = dpl;
    access->p = 1;

    struct gdt_flags * flags = (struct gdt_flags *) &entry->flags_and_limit1;
    flags->limit = 0;
    flags->reserved = 0;
    flags->l = 0;
    flags->db = 0;
    flags->g = 1;

    entry->base2 = 0;
    return (size[cpuid] - 1) * sizeof(struct gdt_entry);
}

uint16_t create_tss_descriptor(uint8_t cpuid, uint64_t base, uint64_t limit) {
    struct gdt_tss_entry *entry = (struct gdt_tss_entry *) &gdt[cpuid][size[cpuid]++];


    entry->limit0 = limit & 0xFFFF;
    entry->base0 = base & 0xFFFF;
    entry->base1 = (base >> 16) & 0xFF;

    struct gdt_system_access_byte *access = (struct gdt_system_access_byte *) &entry->access;
    access->type = GDT_SYSTEM_TYPE_TSS_AVAILABLE;
    access->s = 0;
    access->dpl = 0;
    access->p = 1;

    entry->limit1 = (limit >> 16) & 0xF;
    entry->base2 = (base >> 24) & 0xFF;
    entry->base3 = (base >> 32) & 0xFFFFFFFF;
    entry->reserved = 0;

    size[cpuid]+=2; // TSS takes 2 entries
    return (size[cpuid] - 2) * sizeof(struct gdt_entry);
}

uint16_t create_null_descriptor(uint8_t cpuid) {
    struct gdt_entry *entry = &gdt[cpuid][size[cpuid]++];
    entry->limit0 = 0;
    entry->base0 = 0;
    entry->base1 = 0;

    struct gdt_access_byte *access = (struct gdt_access_byte *) &entry->access;
    access->a = 0;
    access->rw = 0;
    access->dc = 0;
    access->ex = 0;
    access->s = 0;
    access->dpl = 0;
    access->p = 0;

    struct gdt_flags * flags = (struct gdt_flags *) &entry->flags_and_limit1;
    flags->limit = 0;
    flags->reserved = 0;
    flags->l = 0;
    flags->db = 0;
    flags->g = 0;

    entry->base2 = 0;
    return (size[cpuid] - 1) * sizeof(struct gdt_entry);
}

uint16_t get_kernel_code_selector() {
    return GDT_KERNEL_CODE_ENTRY;
}
uint16_t get_kernel_data_selector() {
    return GDT_KERNEL_DATA_ENTRY;
}
uint16_t get_user_code_selector() {
    return GDT_USER_CODE_ENTRY;
}
uint16_t get_user_data_selector() {
    return GDT_USER_DATA_ENTRY;
}

void print_gdt_access(struct gdt_access_byte access) {
    printf("Access byte:\n");
    printf("Accessed: %d\n", access.a);
    printf("Read/Write: %d\n", access.rw);
    printf("Direction/Conforming: %d\n", access.dc);
    printf("Executable: %d\n", access.ex);
    printf("System: %d\n", access.s);
    printf("DPL: %d\n", access.dpl);
    printf("Present: %d\n", access.p);
}

void print_gdt_flags(struct gdt_flags flags) {
    printf("Limit1: %d\n", flags.limit);
    printf("Flags:\n");
    printf("Reserved: %d\n", flags.reserved);
    printf("Long mode: %d\n", flags.l);
    printf("Default operand size: %d\n", flags.db);
    printf("Granularity: %d\n", flags.g);
}

void print_gdt_entry(struct gdt_entry* entry) {
    printf("Entry at: %p\n", entry);
    printf("Limit0: %x\n", entry->limit0);
    printf("Base0: %x\n", entry->base0);
    printf("Base1: %x\n", entry->base1);
    print_gdt_access(entry->access);
    print_gdt_flags(entry->flags_and_limit1);
    printf("Base2: %x\n", entry->base2);
}

void print_system_access_byte(struct gdt_system_access_byte access) {
    printf("System access byte:\n");
    printf("Type: %d\n", access.type);
    printf("S: %d\n", access.s);
    printf("DPL: %d\n", access.dpl);
    printf("P: %d\n", access.p);
}

void print_tss_entry(struct gdt_tss_entry* entry) {
    printf("TSS entry at: %p\n", entry);
    printf("Limit0: %x\n", entry->limit0);
    printf("Base0: %x\n", entry->base0);
    printf("Base1: %x\n", entry->base1);
    print_system_access_byte(entry->access);
    printf("Limit1: %x\n", entry->limit1);
    printf("Base2: %x\n", entry->base2);
    printf("Base3: %x\n", entry->base3);
    printf("Reserved: %x\n", entry->reserved);
}

void debug_gdt(uint8_t cpuid) {
    struct gdt_entry* lgdt = gdt[cpuid];
    printf("GDT at: %p\n", lgdt);
    printf("Null descriptor:\n");
    print_gdt_entry(&lgdt[0]);
    printf("Kernel code:\n");
    print_gdt_entry(&lgdt[1]);
    printf("Kernel data:\n");
    print_gdt_entry(&lgdt[2]);
    printf("User code:\n");
    print_gdt_entry(&lgdt[3]);
    printf("User data:\n");
    print_gdt_entry(&lgdt[4]);
    printf("TSS:\n");
    print_tss_entry((struct gdt_tss_entry*)&lgdt[5]);
}

void create_gdt() {
    for (uint8_t i = 0; i < GDT_MAX_CPU; i++) {
        memset(gdt[i], 0, sizeof(struct gdt_entry) * MAX_GDT_ENTRIES);
        memset(&tss[i], 0, sizeof(struct tss));
        descriptor[i].size = sizeof(struct gdt_entry) * MAX_GDT_ENTRIES - 1;
        descriptor[i].offset = (uint64_t) gdt[i];

        //WARNING: Order matters!!!
        create_null_descriptor(i);
        create_code_descriptor(i, GDT_DPL_KERNEL);
        create_data_descriptor(i, GDT_DPL_KERNEL);
        create_code_descriptor(i, GDT_DPL_USER);
        create_data_descriptor(i, GDT_DPL_USER);
        create_tss_descriptor(i, (uint64_t)tss, sizeof(struct tss));
    }
}

void load_gdt(uint8_t cpuid) {
    if (cpuid >= GDT_MAX_CPU) {
        return;
    }
    _load_gdt(&descriptor[cpuid]);
    __asm__("movw %%ax, %w0\n\t" "ltr %%ax" :: "a" (GDT_TSS_ENTRY));
}

struct tss * get_tss(uint64_t index) {
    if (index >= GDT_MAX_CPU) {
        return 0;
    }
    return &tss[index];
}