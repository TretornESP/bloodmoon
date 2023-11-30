#include "cpus.h"
#include "../memory/paging.h"
#include "../util/printf.h"

void callback(struct bmoon_smp_info *cpu) {
    while (1) {
        __asm__ volatile("hlt");
    }
}

void init_cpus() {
    uint32_t bsp_lapic_id = get_smp_bsp_lapic_id();
    uint64_t cpu_count = get_smp_cpu_count();
    struct bmoon_smp_info **cpus = get_smp_cpus();


    for (uint64_t i = 0; i < cpu_count; i++) {
        if (cpus[i]->lapic_id != bsp_lapic_id) {
            cpus[i]->goto_address = callback;
        }
    }
}
