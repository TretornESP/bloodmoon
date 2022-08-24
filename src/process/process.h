#ifndef _PROCESS_H
#define _PROCESS_H
#include <stdint.h>

#define SAVE_CONTEXT(a) __asm__("movq %%rax, (%0)\n\t"\
                                "movq %%rbx, 8(%0)\n\t"\
                                "movq %%rcx, 16(%0)\n\t"\
                                "movq %%rdx, 24(%0)\n\t"\
                                "movq %%rsi, 32(%0)\n\t"\
                                "movq %%rdi, 40(%0)\n\t"\
                                "movq %%r8, 48(%0)\n\t"\
                                "movq %%r9, 56(%0)\n\t"\
                                "movq %%r10, 64(%0)\n\t"\
                                "movq %%r11, 72(%0)\n\t"\
                                "movq %%r12, 80(%0)\n\t"\
                                "movq %%r13, 88(%0)\n\t"\
                                "movq %%r14, 96(%0)\n\t"\
                                "movq %%r15, 104(%0)\n\t"\
                                "movq %%rbp, 112(%0)\n\t"\
                                "movq %%rsp, 120(%0)\n\t"\
                                : : "m" (a)\
                            );

typedef struct __attribute__((packed)){
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rsp;
    uint64_t rbp;
    uint64_t rip;
} CPU_CONTEXT ;

struct descriptors {
    uint8_t stdin;
    uint8_t stdout;
    uint8_t stderr;
};

struct task {
    int16_t pid;
    int16_t ppid;
    uint8_t priority;
    uint8_t status;
    void * entry;
    CPU_CONTEXT* context;
    struct descriptors* descriptors;
};

void kyieldtest();
void kwritest(const char);
extern void setContext(CPU_CONTEXT*);
extern void getContext(CPU_CONTEXT*);
extern void setContextNoRip(CPU_CONTEXT*);
void dump_context(CPU_CONTEXT*);
#endif