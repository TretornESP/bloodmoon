#include "cpus.h"
#include "../arch/gdt.h"
#include "../proc/syscall.h"
#include "../arch/msr.h"
#include "../arch/tss.h"
#include "../devices/apic/apic.h"
#include "../memory/paging.h"
#include "../io/interrupts.h"
#include "../memory/heap.h"
#include "../util/printf.h"
#include "../util/string.h"
#include "../util/dbgprinter.h"
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
    cpu->tss = get_tss(cpuid);

    cpu->ctx = kmalloc(sizeof(struct cpu_context));
    memset(cpu->ctx, 0, sizeof(struct cpu_context));

    cpu->kstack = kstackalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    memset(cpu->kstack - KERNEL_STACK_SIZE, 0, KERNEL_STACK_SIZE);
    
    void * ist0 = kstackalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    memset(ist0 - KERNEL_STACK_SIZE, 0, KERNEL_STACK_SIZE);
    void * ist1 = kstackalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    memset(ist1 - KERNEL_STACK_SIZE, 0, KERNEL_STACK_SIZE);

    tss_set_stack(lcpu->tss, lcpu->kstack, 0);
    tss_set_ist(cpu->tss, 0, (uint64_t)ist0);
    tss_set_ist(cpu->tss, 1, (uint64_t)ist1);


    reloadGsFs();
    setGsBase((uint64_t) lcpu->ctx);
    load_gdt(cpuid);
    syscall_enable(GDT_KERNEL_CODE_ENTRY * sizeof(gdt_entry_t), GDT_USER_CODE_ENTRY * sizeof(gdt_entry_t));
    load_interrupts_for_local_cpu();
    lcpu->ready = 1;
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

    cpu = kmalloc(sizeof(struct cpu) * cpu_count);
    memset(cpu, 0, sizeof(struct cpu) * cpu_count);

    //Make cpus idle once they are initialized

    for (uint64_t i = 0; i < cpu_count; i++) {
        cpu[i].cid = cpus[i]->lapic_id;
        cpus[i]->goto_address = callback;
    }
    startup_cpu(bsp_lapic_id);

    printf("BSP Reporting for duty!!!\n");
}

struct cpu *get_cpu(uint64_t index) {
    if (index >= cpu_count) {
        return 0;
    }
    if (cpu[index].ready == 0) {
        return 0;
    }
    return &cpu[index];
}

extern uint64_t getGsBase();
uint8_t get_cpu_index() {
    //Get gs base
    uint64_t base = cpu_get_msr_u64(MSR_GS_BASE);
    for (uint64_t i = 0; i < cpu_count; i++) {
        if (cpu[i].ctx == (struct cpu_context *) base) {
            return i;
        }
    }
    panic("CPU not found");
}

struct cpu_context * fork_context(void * init, void * stack) {
    struct cpu_context * ctx = kmalloc(sizeof(struct cpu_context));
    memset(ctx, 0, sizeof(struct cpu_context));
}