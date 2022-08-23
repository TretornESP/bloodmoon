#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <stdint.h>

#define TASK_EXECUTING       0
#define TASK_READY           1
#define TASK_STOPPED         2
#define TASK_UNINTERRUPTIBLE 3
#define TASK_INTERRUPTIBLE   4
#define TASK_ZOMBIE          5
#define TASK_CREATED         6
#define TASK_FREE            7

void dump_processes();
void kwrite(const char*);
void spawn(void*);
void kyield();
void init_scheduler();
#endif