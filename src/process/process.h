#ifndef _PROCESS_H
#define _PROCESS_H
#include <stdint.h>

#define SAVE_CONTEXT(a) __asm__("push %rdi\npush %rax"); getContext(a);

#define __SET_CONTEXT(a) __asm__(\
        "movq 8(%0), %%rbx\n\t"\
        "movq 16(%0), %%rcx\n\t"\
        "movq 24(%0), %%rdx\n\t"\
        "movq 32(%0), %%rsi\n\t"\
        "movq 40(%0), %%rdi\n\t"\
        "movq 48(%0), %%r8\n\t"\
        "movq 56(%0), %%r9\n\t"\
        "movq 64(%0), %%r10\n\t"\
        "movq 72(%0), %%r11\n\t"\
        "movq 80(%0), %%r12\n\t"\
        "movq 88(%0), %%r13\n\t"\
        "movq 96(%0), %%r14\n\t"\
        "movq 104(%0), %%r15\n\t"\
        "movq 112(%0), %%rsp\n\t"\
        "movq 120(%0), %%rbp\n\t"\
        "movq 8(%0), %%rbx\n\t"\
        "movq (%0), %%rax\n\t"\
        : : "r" (a)\
    );

#define SET_CONTEXT(a)\
    register CPU_CONTEXT* cpu __asm__("rax") = a;\
    __SET_CONTEXT(cpu);

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