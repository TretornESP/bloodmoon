#include "scheduler.h"
#include "pit.h"
#include "../memory/heap.h"
#include "../memory/paging.h"
#include "../debugger/dbgshell.h"
#include "../arch/gdt.h"
#include "../util/printf.h"
#include "../util/dbgprinter.h"
#include "../util/string.h"

#define MAX_TASKS 255

struct task *task_head = 0;
struct task * current_task = 0;
struct task * kernel_task = 0;
struct interrupt_frame_error return_frame_error = {0};

//Returns the task that must enter cpu
struct task* schedule() {
    struct task* current = current_task;
    if (current == 0) {
        current = task_head;
    }

    //Try to advance to the next task
    if (current->next != 0) {
        current = current->next;
    } else {
        current = task_head;
    }

    //If the next task is not runnable, try to find a runnable task
    while (current->state != TASK_READY && current->state != TASK_EXECUTING) {
        if (current->next != 0) {
            current = current->next;
        } else {
            current = task_head;
        }
    }

    return current;
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

void yield() {
    struct task * next_task = schedule();
    
    current_task->state = TASK_READY;
    next_task->state = TASK_EXECUTING;
    current_task->pd = swap_pml4(next_task->pd);
    printf("SOY %d\n", current_task->pid);
    if (next_task->context == 0) {
        panic("next_task->context == 0");
    }
    
    swap_context(current_task->context, next_task->context);
    panic("yield() returned");
}

struct task* get_current_task() {
    return current_task;
}

const char * get_current_tty() {
    if (current_task == 0) {
        return 0;
    }
    return current_task->tty;
}

void init() {
    if (requires_preemption()) {
        yield();
    }
    __asm__ __volatile__("int $0x8A"); //Return from kernel
}

void zero_panic() {
    panic("ZERO PANIC");
}

void zero() {
    struct task * task = malloc(sizeof(struct task));
    task->state = TASK_EXECUTING;
    task->flags = 0;
    task->sigpending = 0;

    task->frame = malloc(sizeof(struct interrupt_frame));
    memset(task->frame, 0, sizeof(struct interrupt_frame));
    task->frame->rip = (uint64_t)init;
    task->frame->cs = get_kernel_code_selector();
    task->frame->rsp = (uint64_t)stackalloc(STACK_SIZE);
    task->frame->ss = get_kernel_data_selector();

    struct page_directory* pd = get_pml4();
    uint64_t physaddr = virtual_to_physical(pd, (void*)task->frame->rsp);

    printf("Process stack allocated at 0x%llx\n", physaddr);

    task->nice = 0;
    task->mm = 0;
    task->processor = 0;
    task->sleep_time = 0;
    task->exit_code = 0;
    task->exit_signal = 0;
    task->pdeath_signal = 0;

    task->pid = 0;
    task->ppid = 0;

    task->parent = 0;

    task->uid = 0;
    task->gid = 0;

    task->locks = 0;
    task->open_files = 0;

    task->entry = zero_panic;
    task->context = allocate_process(zero_panic);
    task->pd = duplicate_current_pml4();
    strncpy(task->tty, "default\0", 8);
    task->descriptors = 0;
    task->next = 0;
    task->prev = 0;

    kernel_task = task;
}

void _idle() {
    while (1) {
        printf("b");
    }
}

void idle() {
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

    task->pid = 1;
    task->ppid = 0;
    
    task->parent = 0;

    task->uid = 0;
    task->gid = 0;

    task->locks = 0;
    task->open_files = 0;

    task->entry = _idle;
    task->context = allocate_process(_idle);
    task->pd = duplicate_current_pml4();
    strncpy(task->tty, "default\0", 8);
    task->descriptors = 0;
    task->next = 0;
    task->prev = 0;

    add_task(task);
}

void kernel_loop() {
    while (1) {
        printf("c");
    }
}

void init_scheduler() {
    printf("### SCHEDULER STARTUP ###\n");

    zero(); //Spawn kernel task
    idle(); //Spawn idle task

    printf("### SCHEDULER STARTUP DONE ###\n");
}

void set_current_tty(const char * tty) {
    if (current_task == 0) {
        return;
    }
    memset(current_task->tty, 0, 32);
    strncpy(current_task->tty, tty, strlen(tty));
}

void reset_current_tty() {
    if (current_task == 0) {
        return;
    }
    memset(current_task->tty, 0, 32);
    strncpy(current_task->tty, "default\0", 8);
}

void go() {
    current_task = task_head;
    enable_preemption();
    while (1)
        __asm__ __volatile__("hlt");
}

void save_current_context(struct interrupt_frame * frame) {
    if (kernel_task == 0) {
        panic("kernel_task == 0");
    }
    return_frame_error.error_code = 0x0;
    return_frame_error.cs = frame->cs;
    return_frame_error.ss = frame->ss;
    return_frame_error.rsp = frame->rsp;
    return_frame_error.rip = frame->rip;
    return_frame_error.rflags = frame->rflags;
}

void swap_to_kernel(struct interrupt_frame * frame) {
    if (kernel_task == 0) {
        panic("kernel_task == 0");
    }
    frame->cs = kernel_task->frame->cs;
    frame->ss = kernel_task->frame->ss;
    frame->rsp = kernel_task->frame->rsp;
    frame->rip = kernel_task->frame->rip;
    frame->rflags = kernel_task->frame->rflags;
}

void return_from_kernel(struct interrupt_frame_error* frame) {
    frame->cs = return_frame_error.cs;
    frame->ss = return_frame_error.ss;
    frame->rsp = return_frame_error.rsp;
    frame->rip = return_frame_error.rip;   
    frame->rflags = return_frame_error.rflags;
    frame->error_code = return_frame_error.error_code;
}

void save_current_context_error(struct interrupt_frame_error * frame) {
    if (kernel_task == 0) {
        panic("kernel_task == 0");
    }
    return_frame_error.error_code = frame->error_code;
    return_frame_error.cs = frame->cs;
    return_frame_error.ss = frame->ss;
    return_frame_error.rsp = frame->rsp;
    return_frame_error.rip = frame->rip;
    return_frame_error.rflags = frame->rflags;
}

void swap_to_kernel_error(struct interrupt_frame_error * frame) {
    if (kernel_task == 0) {
        panic("kernel_task == 0");
    }
    memcpy(frame, kernel_task->frame, sizeof(struct interrupt_frame_error));
}