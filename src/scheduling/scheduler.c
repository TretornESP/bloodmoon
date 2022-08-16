#include "scheduler.h"
#include "../process/process.h"
#include "../util/printf.h"
#include "../util/dbgprinter.h"

#define MAX_TASKS 255

struct task kernel_tasks[MAX_TASKS];
uint64_t current_task = 0;

void schedule(CPU_CONTEXT * context) {
    struct task * task = &kernel_tasks[current_task];
    task->context = context;

    struct task * next = &kernel_tasks[++current_task];
    
    while (next->status != TASK_READY) {
        next = &kernel_tasks[current_task++];

        if (current_task > MAX_TASKS) {
            current_task = 0;
        }
    }

    task->status = TASK_READY;
    next->status = TASK_EXECUTING;
    
    setContext(next->context);
}

int8_t spawn(void * entry_addr) {
    struct task * task = &kernel_tasks[current_task];

    uint64_t saved_task = current_task;
    while (task->pid < 0) {
        current_task++;

        if (current_task >= MAX_TASKS) {
            current_task = 0;
        }

        if (current_task == saved_task) {
            printf("Process exhaused!\n");
            return -1;
        }

        task = &kernel_tasks[current_task];
    }

    task->pid = 1; //Changeme
    task->ppid = 1;
    task->priority = 1;
    task->entry = entry_addr;
    task->status = TASK_CREATED;
    task->descriptors = (struct descriptors*)(0x0);

    schedule();
    panic("Returning to spawn!");
    return 0;
}

void kyield() {
    CPU_CONTEXT context;
    getContext(&context);
    schedule(&context);
    panic("Returning to kyield!");
}

void init_scheduler() {
    for (int i = 0; i < MAX_TASKS; i++) {
        kernel_tasks[i].pid = -1;
        kernel_tasks[i].ppid = -1;
        kernel_tasks[i].priority = -1;
        kernel_tasks[i].entry = 0;
        kernel_tasks[i].status = TASK_FREE;
        kernel_tasks[i].descriptors = 0;
    }
}

void kwrite(const char* chr) {
    kyield();
    printf(chr);
}