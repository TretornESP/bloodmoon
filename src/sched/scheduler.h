#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <stdint.h>
#include "../memory/paging.h"
#include "../proc/process.h"
#include "../io/interrupts.h"
#include "../proc/loader.h"
#include "concurrency.h"

#define KERNEL_TASK 0
#define USER_TASK 1

#define TASK_EXECUTING       0
#define TASK_READY           1
#define TASK_STOPPED         2
#define TASK_UNINTERRUPTIBLE 3
#define TASK_INTERRUPTIBLE   4
#define TASK_ZOMBIE          5
#define TASK_CREATED         6
#define TASK_FREE            7
#define TASK_IDLE            8

#define SIGKILL 9

extern atomic_int_t irq_disable_counter;

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

extern void ctxswtch(struct task * old_task, struct task* new_task, void* fxsave, void* fxrstor);
extern void ctxcreat(void* rsp, void* intro, void* fxsave);
extern void uctxcreat(void* rsp, void* intro, void* fxsave);

extern void newctxswtch(struct cpu_context * old_task, struct cpu_context* new_task, void* fxsave, void* fxrstor);
extern void newctxcreat(void* rsp, void* intro);
extern void newuctxcreat(void* rsp, void* intro);

void dump_processes();
struct page_directory * get_new_pd(uint8_t privilege);
struct task* get_current_task();
char * get_current_tty();
void set_current_tty(char *);
void reset_current_tty();
void add_task(struct task* task);
void debug_task(struct task* task, uint64_t original_stack_top);
uint64_t initialize_stack(void * stack, int argc, char* argv[], char *envp[], struct auxv *auxv);
struct task* create_task(void * init_func, const char* tty, uint8_t privilege, int argc, char* argv[], char *envp[], struct auxv *auxv, struct page_directory * startup_pd);
void kill_task(int16_t pid);
void pause_task(struct task* task);
void resume_task(struct task* task);
void init_scheduler();
void pseudo_ps();
void returnoexit();
void exit();
void yield();
void go(uint32_t preempt);
void add_signal(int16_t pid, int signal, void * data, uint64_t size);
void subscribe_signal(int signal, sighandler_t handler);
void process_loop();
void process_signals();

static inline void lock_scheduler(void) {
#ifndef SMP
    atomic_increment(&irq_disable_counter);
#endif
}
 
static inline void unlock_scheduler(void) {
#ifndef SMP
    atomic_decrement(&irq_disable_counter);
#endif
}

#endif