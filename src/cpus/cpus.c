#include "cpus.h"
#include "../arch/gdt.h"
#include "../arch/tss.h"
#include "../dev/apic/apic.h"
#include "../memory/paging.h"
#include "../io/interrupts.h"
#include "../memory/heap.h"
#include "../util/printf.h"
#include "../util/string.h"

extern void reloadGsFs();
extern void setGsBase(uint64_t base);

struct bmoon_smp_info ** cpus;
uint64_t cpu_count;
uint32_t bsp_lapic_id;
struct cpu * cpu;

uint64_t get_smp_cpu_number() {
    return cpu_count;
}

struct bmoon_smp_info *get_smp_cpu(uint64_t index) {
    return cpus[index];
}

struct bmoon_smp_info *get_bsp_cpu() {
    for (uint64_t i = 0; i < cpu_count; i++) {
        if (cpus[i]->lapic_id == bsp_lapic_id) {
            return cpus[i];
        }
    }

    return NULL;
}

uint64_t get_smp_bsp_index() {
    for (uint64_t i = 0; i < cpu_count; i++) {
        if (cpus[i]->lapic_id == bsp_lapic_id) {
            return i;
        }
    }

    return 0;
}

void startup_cpu(uint8_t cpuid) {

    printf("Booting CPU %d\n", cpuid);

    struct cpu * lcpu = &cpu[cpuid];
    lcpu->tss = get_tss(cpuid);
    lcpu->ctx = malloc(sizeof(struct cpu_context));
    memset(lcpu->ctx, 0, sizeof(struct cpu_context));
    lcpu->ustack = stackalloc(USER_STACK_SIZE) + USER_STACK_SIZE;
    memset(lcpu->ustack - USER_STACK_SIZE, 0, USER_STACK_SIZE);
    lcpu->kstack = stackalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    memset(lcpu->kstack - KERNEL_STACK_SIZE, 0, KERNEL_STACK_SIZE);
    
    tss_set_stack(lcpu->tss, lcpu->kstack, 0);
    tss_set_stack(lcpu->tss, lcpu->ustack, 3);

    reloadGsFs();
    setGsBase((uint64_t) lcpu->ctx);

    load_gdt(cpuid);
    load_interrupts_for_local_cpu();
}

void callback(struct bmoon_smp_info *cpu) {
    while (1) {
        __asm__ volatile("hlt");
    }
}

void init_cpus() {
    //Only bsp will initialize the cpus until the last step
    //The write to goto_address makes the other cpus startup
    bsp_lapic_id = get_smp_bsp_lapic_id();
    cpu_count = get_smp_cpu_count();
    cpus = get_smp_cpus();

    cpu = malloc(sizeof(struct cpu) * cpu_count);
    memset(cpu, 0, sizeof(struct cpu) * cpu_count);

    //Make cpus idle once they are initialized

    for (uint64_t i = 0; i < cpu_count; i++) {
        cpu[i].cid = cpus[i]->lapic_id;
        cpus[i]->goto_address = callback;
    }
    startup_cpu(bsp_lapic_id);

    printf("BSP Reporting for duty!!!\n");
    struct madt_header* madt = get_acpi_madt();
    if (madt != 0) {
        register_apic(madt, 0x0);
    }
}

struct cpu *get_cpu(uint64_t index) {
    return &cpu[index];
}
