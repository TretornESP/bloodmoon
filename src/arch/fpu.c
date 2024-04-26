#include "fpu.h"

#define FPU_ENABLE_CODE 0x200
#define FPU_CONTROL_WORD 0x37F

#include <stdint.h>

void init_fpu() {
    uint64_t cr0;
    uint16_t control_word = FPU_CONTROL_WORD;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= FPU_ENABLE_CODE;
    __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
    __asm__ volatile("fldcw %0" : : "m"(control_word));
}