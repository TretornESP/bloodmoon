#include "msr.h"
 
void cpuGetMSR(uint32_t msr, uint32_t *lo, uint32_t *hi) {
   __asm__ volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}
 
void cpuSetMSR(uint32_t msr, uint32_t lo, uint32_t hi) {
   __asm__ volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}

void cpuGetMSR64(uint32_t msr, uint64_t *val) {
   uint32_t lo, hi;
   cpuGetMSR(msr, &lo, &hi);
   *val = ((uint64_t)hi << 32) | lo;
}
void cpuSetMSR64(uint32_t msr, uint64_t val) {
   cpuSetMSR(msr, val & 0xFFFFFFFF, val >> 32);
}