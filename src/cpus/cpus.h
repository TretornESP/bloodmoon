#ifndef _CPUS_H
#define _CPUS_H

#include "../bootservices/bootservices.h"
#include <stdint.h>

struct bmoon_smp_info *get_smp_cpu(uint64_t index);
uint64_t get_smp_cpu_number();
struct bmoon_smp_info *get_bsp_cpu();
uint64_t get_smp_bsp_index();

void init_cpus();

#endif