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

struct interrupt_frame_error return_frame_error = {0};

//Returns the task that must enter cpu

void __attribute__((optimize("omit-frame-pointer"))) schedule() {

    __asm__ volatile("cli");
    __asm__ volatile("pushq %rbx");
    __asm__ volatile("pushq %r12");
    __asm__ volatile("pushq %r13");
    __asm__ volatile("pushq %r14");
    __asm__ volatile("pushq %r15");
    
    __asm__ volatile("movq %%rsp, %0" : "=r"(current_task->rsp_top));

    struct task * next_task = 0;
    struct task* current = current_task;
    struct task* original = current_task;

    if (current == 0) {
        next_task = task_head;
    }
    
    //Find the next task
    while (next_task == 0) {
        if (current->next == 0) {
            current = task_head;
        } else {
            current = current->next;
        }

        if (current->state == TASK_READY) {
            next_task = current;
        }

        if (current == original) {
            break;
        }
    }

    if (next_task == 0) {
        return;
    }

    struct task current_task_copy;
    memcpy(&current_task_copy, current_task, sizeof(struct task));

    current_task->state = TASK_READY;
    current_task = next_task;
    current_task->state = TASK_EXECUTING;

    swap_pml4(current_task->pd);
    tss_set_stack(current_task->processor, (void*)current_task->rsp_top, 0);
    __asm__ volatile("movq %0, %%rsp" : : "r"(current_task->rsp_top));

    __asm__ volatile("popq %r15");
    __asm__ volatile("popq %r14");
    __asm__ volatile("popq %r13");
    __asm__ volatile("popq %r12");
    __asm__ volatile("popq %rbx");
    
    __asm__ volatile("sti");
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
        printf("PID: %d PPID: %d STATE: %d FLAGS: %d, ENTRY: %p TTY: %s\n", current->pid, current->ppid, current->state, current->flags, current->entry, current->tty);
        current = current->next;
    }

    return;
}

struct task* get_current_task() {
    return current_task;
}

void go() {
    struct task* current = get_current_task();
    __asm__ volatile("cli");
    current->state = TASK_EXECUTING;

    swap_pml4(current->pd);
    tss_set_stack(current->processor, (void*)current->rsp_top, 0);
    __asm__ volatile("movq %0, %%rsp" : : "r"(current->rsp_top));

    __asm__ volatile("popq %r15");
    __asm__ volatile("popq %r14");
    __asm__ volatile("popq %r13");
    __asm__ volatile("popq %r12");
    __asm__ volatile("popq %rbx");

    __asm__ volatile("sti");
}

char * get_current_tty() {
    if (current_task == 0) {
        return 0;
    }
    return current_task->tty;
}

void atask() {
    while (1) {
        printf("a");
        //Set r12-r15 to 0x12, 0x13, 0x14, 0x15
        __asm__ volatile("movq $0x12, %r12");
        __asm__ volatile("movq $0x13, %r13");
        __asm__ volatile("movq $0x14, %r14");
        __asm__ volatile("movq $0x15, %r15");
        __asm__ volatile("movq $0x69, %rbx");
        schedule();
    }
}

void btask() {
    while (1) {
        __asm__ volatile("movq $0x21, %r12");
        __asm__ volatile("movq $0x31, %r13");
        __asm__ volatile("movq $0x41, %r14");
        __asm__ volatile("movq $0x51, %r15");
        __asm__ volatile("movq $0x96, %rbx");
        printf("b");
        schedule();
    }
}

void spawn_task(void * init_func) {
    struct task * task = malloc(sizeof(struct task));
    task->state = TASK_READY;
    task->flags = 0;
    task->sigpending = 0;

    task->nice = 0;
    task->mm = 0;

    task->frame = 0;

    task->processor = 0;
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
    //Set rbp to 0
    //Set rip to init_func

    uint64_t zero = 0;

    uint64_t * saved_rsp;
    //Save current rsp
    __asm__ volatile("movq %%rsp, %0" : "=r"(saved_rsp));

    //Set rsp to the top of the stack
    __asm__ volatile("movq %0, %%rsp" : : "r"(task->rsp_top));

    //Push the return address
    __asm__ volatile("pushq %0" : : "r"(init_func));

    //Push rbp
    __asm__ volatile("pushq %0" : : "r"(zero));

    __asm__ volatile("pushq $0x99"); //RBX
    __asm__ volatile("pushq $0x92"); //R12
    __asm__ volatile("pushq $0x93"); //R13
    __asm__ volatile("pushq $0x94"); //R14
    __asm__ volatile("pushq $0x95"); //R15

    //Set rsp to the saved rsp
    __asm__ volatile("movq %0, %%rsp" : : "r"(saved_rsp));
    
    //Update rsp_top to account for all the pushes
    task->rsp_top = task->rsp_top - 0x8 * 7;

    task->pd = duplicate_current_pml4();
    strncpy(task->tty, "default\0", 8);
    task->descriptors = 0;
    task->next = 0;
    task->prev = 0;

    add_task(task);
}

void init_scheduler() {
    printf("### SCHEDULER STARTUP ###\n");

    spawn_task(atask); //Spawn idle task
    spawn_task(btask); //Spawn idle task

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

void reset_current_tty() {
    if (current_task == 0) {
        return;
    }
    memset(current_task->tty, 0, 32);
    strncpy(current_task->tty, "default\0", 8);
}