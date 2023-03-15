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

struct vm_area_struct {
    struct mm_struct *vm_mm;        /* The address space we belong to. */
    unsigned long vm_start;         /* Our start address within vm_mm. */
    unsigned long vm_end;           /* The first byte after our end address
                                       within vm_mm. */
    struct vm_area_struct *vm_next; /* linked list of VM areas per task */
    unsigned long vm_flags;         /* Flags, listed below. */
    unsigned long vm_page_prot;     //This is arch specific protection bits, unused by now
    int file;
};

struct mm_struct {
        struct vm_area_struct  *mmap;               /* list of memory areas */
        unsigned long long     pgd;                /* page global directory */
        int                    mm_users;            /* address space users */
        int                    map_count;           /* number of memory areas */

        unsigned long          start_code;          /* start address of code */
        unsigned long          end_code;            /* final address of code */
        unsigned long          start_data;          /* start address of data */
        unsigned long          end_data;            /* final address of data */
        unsigned long          start_brk;           /* start address of heap */
        unsigned long          brk;                 /* final address of heap */
        unsigned long          start_stack;         /* start address of stack */
        unsigned long          arg_start;           /* start of arguments */
        unsigned long          arg_end;             /* end of arguments */
        unsigned long          env_start;           /* start of environment */
        unsigned long          env_end;             /* end of environment */

        unsigned long          rss;                 /* pages allocated */
        unsigned long          total_vm;            /* total number of pages */
        unsigned long          locked_vm;           /* number of locked pages */
};

struct task {
    volatile long state;
    unsigned long flags;
    int sigpending;
    
    long nice;
    struct mm_struct *mm;
    int processor;
    unsigned long sleep_time;
    int exit_code, exit_signal;
    int pdeath_signal;

    int16_t pid;
    int16_t ppid;

    struct task *parent;

    int16_t uid;
    int16_t gid;

    //struct tty_struct *tty; TODO: Add tty support
    unsigned int locks;

    int * open_files;

    void * entry;
    CPU_CONTEXT* context;
    struct descriptors* descriptors;
    struct task *next, *prev;

};

void kyieldtest();
void kwritest(const char);
extern void setContext(CPU_CONTEXT*);
extern void getContext(CPU_CONTEXT*);
extern void setContextNoRip(CPU_CONTEXT*);
void dump_context(CPU_CONTEXT*);

#endif