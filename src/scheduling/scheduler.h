#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <stdint.h>
#include "../memory/paging.h"
#include "../process/process.h"
#include "../io/interrupts.h"

#define TASK_EXECUTING       0
#define TASK_READY           1
#define TASK_STOPPED         2
#define TASK_UNINTERRUPTIBLE 3
#define TASK_INTERRUPTIBLE   4
#define TASK_ZOMBIE          5
#define TASK_CREATED         6
#define TASK_FREE            7

//x86_64 system v abi calling convention stack frame
struct stack_frame {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12; 
    uint64_t r11; 
    uint64_t r10; 
    uint64_t r9; 
    uint64_t r8; 
    uint64_t rbp;
    uint64_t rdi; 
    uint64_t rsi; 
    uint64_t rdx; 
    uint64_t rcx; 
    uint64_t rbx; 
    uint64_t rax;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed));

extern void save_context(struct task* task);

void dump_processes();
struct task* get_current_task();
char * get_current_tty();
void set_current_tty(char *);
void reset_current_tty();
void init_scheduler();
void pseudo_ps();
void go();
void schedule();
#endif