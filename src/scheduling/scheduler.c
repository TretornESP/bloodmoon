#include "scheduler.h"
#include "pit.h"
#include "../memory/heap.h"
#include "../memory/paging.h"
#include "../arch/gdt.h"
#include "../util/printf.h"
#include "../util/dbgprinter.h"
#include "../util/string.h"
#include "../arch/tss.h"

#define MAX_TASKS 255

struct task *task_head = 0;
struct task * current_task = 0;
struct task * shithole_task = 0;

int irq_disable_counter = 0;

//Returns the task that must enter cpu

struct task * schedule() {

    struct task * next_task = 0;
    struct task* current = current_task;
    struct task* original = current_task;

    //Find a task that is ready to run TASK_READY, if not found, run idle task
    while (next_task == 0) {
        if (current->next == 0) {
            current = task_head;
        } else {
            current = current->next;
        }

        if (current->state == TASK_READY || current->state == TASK_EXECUTING) {
            next_task = current;
        }

        if (current == original) {
            break;
        }
    }

    if (next_task == 0) {
        printf("No tasks found, running idle task\n");
        next_task = task_head;
        if (next_task->state != TASK_IDLE) {
            panic("Idle task is not valid or at head\n");
        }
    }

    if (next_task == 0) {
        panic("No tasks found, not even idle!!!\n");
    }

    struct task * prev_task = current_task;  

    if (current_task->state == TASK_EXECUTING)  
        current_task->state = TASK_READY;
    
    current_task = next_task;

    if (current_task->state == TASK_READY)
        current_task->state = TASK_EXECUTING;

    //printf("Switching from task %d to %d\n", prev_task->pid, current_task->pid);

    return prev_task;
}

void lock_scheduler(void) {
#ifndef SMP
    __asm__("cli");
    irq_disable_counter++;
#endif
}
 
void unlock_scheduler(void) {
#ifndef SMP
    irq_disable_counter--;
    if(irq_disable_counter == 0) {
        __asm__("sti");
    }
#endif
}

void block_task(long reason) {
    lock_scheduler();
    current_task->state = reason;
    yield();
    unlock_scheduler();
}

void unblock_task(struct task * task) {
    lock_scheduler();
    task->state = TASK_READY;
    yield();
    unlock_scheduler();
}

void __attribute__((noinline)) yield() {
    if (current_task->last_scheduled != 0)
        current_task->cpu_time += (get_ticks_since_boot() - current_task->last_scheduled);

    struct task * prev = schedule();

    current_task->last_scheduled = get_ticks_since_boot();

    tss_set_stack(current_task->processor, (void*)current_task->rsp, 0);
    ctxswtch(
        prev,
        current_task
    );
}

void add_task(struct task* task) {
    printf("Adding task %d\n", task->pid);

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
    int16_t pid = 1;
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
        printf("PID: %d PPID: %d STATE: %d FLAGS: %d, ENTRY: %p TTY: %s\n", current->pid, current->ppid, current->state, current->flags, current->entry, current->tty);
        current = current->next;
    }

    return;
}

struct task* get_current_task() {
    return current_task;
}

//void go(uint8_t preempt) {
void go(uint8_t preempt) {

    shithole_task = malloc(sizeof(struct task));
    memset(shithole_task, 0, sizeof(struct task));
    schedule();
    if (preempt) {
        __asm__("cli");
        set_preeption_ticks(preempt);
        enable_preemption();
    }
    tss_set_stack(current_task->processor, (void*)current_task->rsp, 0);
    ctxswtch(
        shithole_task,
        current_task
    );
    panic("go returned you melon!");
}

char * get_current_tty() {
    if (current_task == 0) {
        return 0;
    }
    return current_task->tty;
}

void idle() {
    while (1) {
        __asm__ volatile("hlt");
    }
}

void exit() {
    //printf("Exiting task %d\n", current_task->pid);
    current_task->state = TASK_ZOMBIE;
    yield();
}

void kill_task(int16_t pid) {
    struct task* task = get_task(pid);
    if (task == 0) {
        printf("No such task\n");
        return;
    }

    printf("Killing task %d\n", pid);
    task->exit_code = 0;
    task->exit_signal = SIGKILL;
    task->state = TASK_ZOMBIE;
}

void atask() {
    int i = 10;
    while (i--) {
        printf("atask: %d\n", i);
        yield();
    }
    exit();
}

void btask() {
    int i = 20;
    while (i--) {
        printf("btask: %d\n", i);
        yield();
    }
    exit();
}

struct task* create_task(void * init_func, const char * tty) {
    struct task * task = malloc(sizeof(struct task));
    task->state = TASK_READY;
    task->flags = 0;
    task->sigpending = 0;

    task->nice = 0;
    task->mm = 0;

    task->frame = 0;

    task->processor = 0;
    task->cpu_time = 0;
    task->last_scheduled = 0;
    task->sleep_time = 0;
    task->exit_code = 0;    
    task->exit_signal = 0;
    task->pdeath_signal = 0;

    struct task * parent = get_current_task();
    if (parent == 0) {
        parent = task;
        task->ppid = 0;
    } else {
        task->ppid = parent->pid;
    }

    task->parent = parent;
    task->pid = get_free_pid();

    task->uid = 0;
    task->gid = 0;

    task->locks = 0;
    task->open_files = 0;

    task->entry = init_func;
    task->rsp = (uint64_t)stackalloc(STACK_SIZE);
    task->rsp_top = task->rsp + STACK_SIZE;

    //Create a stack frame (do not use structs)
    //Set r12-r15 to 0, rbx to 0
    //Set rip to init_func

    uint64_t * saved_rsp;
    //Save current rsp
    __asm__ volatile("movq %%rsp, %0" : "=r"(saved_rsp));

    //Set rsp to the top of the stack
    __asm__ volatile("movq %0, %%rsp" : : "r"(task->rsp_top));

    //Push the return address
    __asm__ volatile("pushq %0" : : "r"(init_func));

    //Push the rsp_top
    __asm__ volatile("pushq %0" : : "r"(task->rsp_top));

    __asm__ volatile("pushq $0x99"); //RBX
    __asm__ volatile("pushq $0x92"); //R12
    __asm__ volatile("pushq $0x93"); //R13
    __asm__ volatile("pushq $0x94"); //R14
    __asm__ volatile("pushq $0x95"); //R15

    //Save rsp to the task struct
    __asm__ volatile("movq %%rsp, %0" : "=r"(task->rsp_top));
    //Set rsp to the saved rsp
    __asm__ volatile("movq %0, %%rsp" : : "r"(saved_rsp));

    task->pd = duplicate_current_pml4();
    strncpy(task->tty, tty, strlen(tty));
    task->descriptors = 0;
    task->next = 0;
    task->prev = 0;

    printf("Created task for address %p\n", init_func);

    return task;
}

void init_scheduler() {
    printf("### SCHEDULER STARTUP ###\n");
    struct task * idle_task = create_task(idle, "default"); //Spawn idle task
    idle_task->pid = 0;
    idle_task->state = TASK_IDLE;

    add_task(idle_task);

    current_task = task_head;

    printf("### SCHEDULER STARTUP DONE ###\n");
}

void set_current_tty(char * tty) {
    if (current_task == 0) {
        return;
    }
    memset(current_task->tty, 0, 32);
    strncpy(current_task->tty, tty, strlen(tty));
    //Make sure it's null terminated
    current_task->tty[strlen(tty)] = '\0';
}

void task_test() {
    printf("Task a is at: %p\n", &atask);
    printf("Task b is at: %p\n", &btask);
}

void reset_current_tty() {
    if (current_task == 0) {
        return;
    }
    memset(current_task->tty, 0, 32);
    strncpy(current_task->tty, "default\0", 8);
}