#ifndef _PROCESS_H
#define _PROCESS_H
#include <stdint.h>
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

extern void setContext(CPU_CONTEXT*);
extern void getContext(CPU_CONTEXT*);

#endif