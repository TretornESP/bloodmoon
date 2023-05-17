#ifndef _TSS_H
#define _TSS_H
#include <stdint.h>

#define TSS_DEFAULT_CPU 0x0
#define TSS_IST_STACK_SIZE 0x10000 // 64KB
#define CPU_MAX 256
#define TSS_RSP_SIZE 3
#define TSS_IST_SIZE 7

struct tss {
    uint32_t reserved0;
    uint64_t rsp[TSS_RSP_SIZE];
    uint64_t reserved1;
    uint64_t ist[TSS_IST_SIZE];
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iopb;
} __attribute__((packed));

void tss_init();
struct tss * get_tss(uint8_t cpu);
struct tss * get_default_tss();
void prepare_tss(struct tss* tss);
#endif
