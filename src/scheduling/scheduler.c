#include "scheduler.h"
#include "../process/process.h"
#include "../util/printf.h"
#include "../util/dbgprinter.h"

#define MAX_TASKS 255

struct task kernel_tasks[MAX_TASKS];
uint64_t current_task = 0;

//Returns the task that must enter cpu
uint64_t schedule() {
    uint64_t i = current_task+1;
    struct task * next = &kernel_tasks[i];
    
    while (next->status != TASK_READY) {
        next = &kernel_tasks[++i];

        if (i > MAX_TASKS) {
            i = 0;
        }

        if (i == current_task) {
            printf("No tasks!\n");
            return -1; //No task available! TODO: Modify this
        }
    }

    printf("Scheduled for task: %d\n", i);
    return i;
}

void dump_processes() {
    for (uint8_t tid = 0; tid < MAX_TASKS; tid++) {
        struct task * task = &kernel_tasks[tid];
        if (task->status != TASK_FREE) {
            printf("Task %d [PID: %d ENTRY: %p STATUS: %d]\n", tid, task->pid, task->entry, task->status);
        }
    }
}

void spawn(void * entry_addr) {
    struct task * task = &kernel_tasks[current_task];
    task->status = TASK_READY;
    uint64_t i = current_task+1;
    struct task * new_task = &kernel_tasks[i];

    while (new_task->status != TASK_FREE) {
        new_task = &kernel_tasks[++i];

        if (i >= MAX_TASKS) {
            i = 0;
        }

        if (i == current_task) {
            panic("Process exhaused!");
        }
    }

    new_task->pid = 1; //Changeme
    new_task->ppid = 1;
    new_task->priority = 1;
    new_task->entry = entry_addr;
    new_task->status = TASK_EXECUTING;
    new_task->descriptors = (struct descriptors*)(0x0);

    void (*entry)() = entry_addr;
    entry();
    panic("Process returned!");
}

void kyield() {
    printf("TASK %d YIELDING\n", current_task);
    CPU_CONTEXT context;
    getContext(&context);
    dump_context(&context);

    struct task * task = &kernel_tasks[current_task];
    task->context = &context;
    
    task->status = TASK_READY;
    task = &kernel_tasks[schedule()];
    task->status = TASK_EXECUTING;
    setContext(task->context);

    panic("Returning to kyield!");
}

void init_scheduler() {
    printf("### SCHEDULER STARTUP ###\n");
    kernel_tasks[0].pid = 0;
    kernel_tasks[0].ppid = 0;
    kernel_tasks[0].priority = 0;
    kernel_tasks[0].entry = (void*)0xdeadbeef;
    kernel_tasks[0].status = TASK_EXECUTING;
    kernel_tasks[0].descriptors = (struct descriptors*)(0x0);

    CPU_CONTEXT context;
    getContext(&context);
    kernel_tasks[0].context = &context;

    for (int i = 1; i < MAX_TASKS; i++) {
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