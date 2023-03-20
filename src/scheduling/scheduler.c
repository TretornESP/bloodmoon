#include "scheduler.h"
#include "../process/process.h"
#include "../memory/heap.h"
#include "../util/printf.h"
#include "../util/dbgprinter.h"
#include "../util/string.h"

#define MAX_TASKS 255

struct task *task_head = 0;
struct task * current_task = 0;

//Returns the task that must enter cpu
uint64_t schedule() {
}

void add_task(struct task* task) {
    if (task_head == 0) {
        task_head = task;
        return;
    }

    struct task* current = task_head;
    while (current->next != 0) {
        current = current->next;
    }

    current->next = task;
    task->prev = current;
}

void remove_task(struct task* task) {
    if (task->prev == 0) {
        task_head = task->next;
        if (task_head != 0) {
            task_head->prev = 0;
        }
        return;
    }

    task->prev->next = task->next;
    if (task->next != 0) {
        task->next->prev = task->prev;
    }
}

struct task* get_task(int16_t pid) {
    struct task* current = task_head;
    if (current == 0) {
        return 0;
    }

    if (current->pid == pid) {
        return current;
    }

    while (current->next != 0) {
        if (current->next->pid == pid) {
            return current->next;
        }
        current = current->next;
    }

    return 0;
}

struct task* get_status(long state) {
    struct task* current = task_head;
    if (current == 0) {
        return 0;
    }

    if (current->state == state) {
        return current;
    }

    while (current->next != 0) {
        if (current->next->state == state) {
            return current->next;
        }
        current = current->next;
    }

    return 0;
}

int16_t get_free_pid() {
    int16_t pid = 0;
    while (get_task(pid) != 0) {
        pid++;
    }

    return pid;
}

void pseudo_ps() {
    struct task* current = task_head;
    if (current == 0) {
        return;
    }

    while (current != 0) {
        printf("PID: %d PPID: %d STATE: %d FLAGS: %d, ENTRY: %p\n", current->pid, current->ppid, current->state, current->flags, current->entry);
        current = current->next;
    }

    return;
}

void spawn(
    uint16_t parent_pid,
    long niceness,
    unsigned long flags,
    void* entry_addr,
    long uid,
    long gid
) {
    struct task * task = malloc(sizeof(struct task));
    task->state = TASK_READY;
    task->flags = flags;
    task->sigpending = 0;
    task->nice = niceness;
    task->mm = 0;//TODO: struct mm
    task->processor = 0;//TODO: smp
    task->sleep_time = 0;
    task->exit_code = 0;
    task->exit_signal = 0;
    task->pdeath_signal = 0;
    task->pid = get_free_pid();
    task->ppid = parent_pid;
    task->parent = get_task(parent_pid);
    task->uid = uid;
    task->gid = gid;
    task->locks = 0;
    task->open_files = 0;
    task->entry = entry_addr;
    
    CPU_CONTEXT * context = malloc(sizeof(CPU_CONTEXT));
    memset(context, 0, sizeof(CPU_CONTEXT));
    task->context = context;
    task->descriptors = 0;//TODO: struct descriptors
    task->next = 0;
    task->prev = 0;
    add_task(task); 
}

void kyield() {
    /*printf("TASK %d YIELDING\n", current_task);
    CPU_CONTEXT context;
    getContext(&context);
    dump_context(&context);

    struct task * task = &kernel_tasks[current_task];
    task->context = &context;
    
    task->status = TASK_READY;
    task = &kernel_tasks[schedule()];
    task->status = TASK_EXECUTING;
    setContext(task->context);
*/
    panic("kyield!");
}

struct task* get_current_task() {
    return current_task;
}

void init_scheduler() {
    printf("### SCHEDULER STARTUP ###\n");

    spawn(0, 0, 0, (void*)0x0, 0, 0); //Spawn kernel task

    current_task = task_head;
    current_task->state = TASK_EXECUTING;
    printf("### SCHEDULER STARTUP DONE ###\n");
}

void kwrite(const char* chr) {
    kyield();
    printf(chr);
}