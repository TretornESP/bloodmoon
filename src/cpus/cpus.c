#include "cpus.h"
#include "../memory/paging.h"
#include "../util/printf.h"

struct bmoon_smp_info ** cpus;
uint64_t cpu_count;
uint32_t bsp_lapic_id;

void callback(struct bmoon_smp_info *cpu) {
    while (1) {
        __asm__ volatile("hlt");
    }
}

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

void init_cpus() {
    bsp_lapic_id = get_smp_bsp_lapic_id();
    cpu_count = get_smp_cpu_count();
    cpus = get_smp_cpus();


    for (uint64_t i = 0; i < cpu_count; i++) {
        if (cpus[i]->lapic_id != bsp_lapic_id) {
            cpus[i]->goto_address = callback;
        }
    }
}
