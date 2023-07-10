#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <stdint.h>
#include "../memory/paging.h"
#include "../process/process.h"

#define TASK_EXECUTING       0
#define TASK_READY           1
#define TASK_STOPPED         2
#define TASK_UNINTERRUPTIBLE 3
#define TASK_INTERRUPTIBLE   4
#define TASK_ZOMBIE          5
#define TASK_CREATED         6
#define TASK_FREE            7

extern void swap_context(CPU_CONTEXT* old, CPU_CONTEXT* new);
void dump_processes();
struct task* get_current_task();
const char * get_current_tty();
void set_current_tty(const char *);
void reset_current_tty();
void spawn(long, unsigned long, long, long);
void yield();
void init_scheduler();
void pseudo_ps();
#endif