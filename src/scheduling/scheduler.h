#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <stdint.h>
#include "../memory/paging.h"
#include "../process/process.h"
#include "../io/interrupts.h"
#include "concurrency.h"

#define TASK_EXECUTING       0
#define TASK_READY           1
#define TASK_STOPPED         2
#define TASK_UNINTERRUPTIBLE 3 //Paused, cannot be interrupted
#define TASK_INTERRUPTIBLE   4 //Paused, can be interrupted
#define TASK_ZOMBIE          5
#define TASK_CREATED         6
#define TASK_FREE            7
#define TASK_IDLE            8

#define SIGKILL 9
#define SCHED_PRIORITIES 5
#define SCHED_AGE_THRESHOLD 500 //1 second

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


extern lock_t task_lock;
extern void ctxswtch(struct task * old_task, struct task* new_task, void* fxsave, void* fxrstor);
extern void ctxcreat(void* rsp, void* intro, void* fxsave);
void dump_processes();
struct task* get_current_task();
char * get_current_tty();
void set_current_tty(char *);
void reset_current_tty();
void add_task(struct task* task);
struct task* create_task(void * init_func, long nice, const char * tty);
void kill_task(int16_t pid);
void pause_task(struct task* task);
void dump_task_queues();
void resume_task(struct task* task);
void init_scheduler();
void pseudo_ps();
void returnoexit();
void exit();
void yield();
void task_test();
void go(uint32_t preempt);
void add_signal(int16_t pid, int signal, void * data, uint64_t size);
void subscribe_signal(int signal, sighandler_t handler);
void process_signals();

#define DEBUG_LOCKS 1

#ifndef DEBUG_LOCKS
#define DEBUG_RED(x)
#else
#define DEBUG_RED(x) RED(x)
#endif

//This is a read-write lock
#define lock_scheduler() \
    DEBUG_RED("lock_scheduler") \
    LOCK(&task_lock); \
    atomic_increment(&irq_disable_counter);

#define lock_scheduler_ns() \
    DEBUG_RED("lock_scheduler_ns") \
    LOCK_NS(&task_lock); \
    atomic_increment(&irq_disable_counter);

//This is a read_only lock
#define lock_scheduler_explicit() \
    atomic_increment(&irq_disable_counter);

#define unlock_scheduler() \
    DEBUG_RED("unlock_scheduler") \
    UNLOCK(&task_lock); \
    atomic_decrement(&irq_disable_counter);

#define unlock_scheduler_ns() \
    DEBUG_RED("unlock_scheduler_ns") \
    UNLOCK_NS(&task_lock); \
    atomic_decrement(&irq_disable_counter);

#define unlock_scheduler_explicit() \
    atomic_decrement(&irq_disable_counter);

#endif