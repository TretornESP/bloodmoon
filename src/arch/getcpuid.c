#include <cpuid.h>
#include "getcpuid.h"

#define CPUID_GETMODEL(cpuinfo) ((cpuinfo[0] & 0xf0) >> 4)
#define CPUID_GETAPIC(cpuinfo) ((cpuinfo[3] & (1 << 9)) != 0)

/* Example: Get CPU's model number */
int get_model(void)
{
    int cpuinfo[4];
    __cpuid(1, cpuinfo[0], cpuinfo[1], cpuinfo[2], cpuinfo[3]);
    return CPUID_GETMODEL(cpuinfo);
}

/* Example: Get APIC capabilities */
int check_apic(void)
{
    int cpuinfo[4];
    __cpuid(1, cpuinfo[0], cpuinfo[1], cpuinfo[2], cpuinfo[3]);
    return (CPUID_GETAPIC(cpuinfo));
}