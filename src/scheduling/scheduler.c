#include "scheduler.h"
#include "pit.h"
#include "../cpus/cpus.h"
#include "../memory/heap.h"
#include "../memory/paging.h"
#include "../arch/simd.h"
#include "../arch/gdt.h"
#include "../util/printf.h"
#include "../util/dbgprinter.h"
#include "../dev/devices.h"
#include "../util/string.h"
#include "../arch/tss.h"
#include "../vfs/vfs_interface.h"
#include "../cpus/cpus.h"

#define MAX_TASKS 255

atomic_int_t irq_disable_counter = 0;

struct task *task_head = 0;
struct task * current_task = 0;
struct task * shithole_task = 0;

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
        //printf("No tasks found, running idle task\n");
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

void block_task(long reason) {
    printf("Blocking task %d\n", current_task->pid);
    lock_scheduler();
    current_task->state = reason;
    yield();
    unlock_scheduler();
}

void unblock_task(struct task * task) {
    printf("Unblocking task %d\n", task->pid);
    lock_scheduler();
    task->state = TASK_READY;
    yield();
    unlock_scheduler();
}

void __attribute__((noinline)) yield() {
    if (irq_disable_counter) return;
    if (current_task->last_scheduled != 0)
        current_task->cpu_time += (get_ticks_since_boot() - current_task->last_scheduled);

    struct task * prev = schedule();
    //printf("Switching from task %d to %d\n", prev->pid, current_task->pid);

    current_task->last_scheduled = get_ticks_since_boot();

    struct cpu * cpu = get_cpu(current_task->processor);
    tss_set_stack(cpu->tss, (void*)current_task->stack_top, 0);
    ctxswtch(
        prev,
        current_task,
        prev->fxsave_region,
        current_task->fxsave_region
    );

    process_signals();
}

void add_task(struct task* task) {
    lock_scheduler();
    printf("Adding task %d\n", task->pid);

    if (task_head == 0) {
        task_head = task;
        unlock_scheduler();
        return;
    }

    struct task* current = task_head;
    while (current->next != 0) {
        current = current->next;
    }

    current->next = task;
    task->prev = current;
    unlock_scheduler();
}

void remove_task(struct task* task) {
    lock_scheduler();
    if (task->prev == 0) {
        task_head = task->next;
        if (task_head != 0) {
            task_head->prev = 0;
        }
        unlock_scheduler();
        return;
    }

    task->prev->next = task->next;
    if (task->next != 0) {
        task->next->prev = task->prev;
    }
    unlock_scheduler();
}

struct task* get_task(int16_t pid) {
    lock_scheduler();
    struct task* current = task_head;
    if (current == 0) {
        unlock_scheduler();
        return 0;
    }

    if (current->pid == pid) {
        unlock_scheduler();
        return current;
    }

    while (current->next != 0) {
        if (current->next->pid == pid) {
            unlock_scheduler();
            return current->next;
        }
        current = current->next;
    }

    unlock_scheduler();
    return 0;
}

struct task* get_status(long state) {
    lock_scheduler();
    struct task* current = task_head;
    if (current == 0) {
        unlock_scheduler();
        return 0;
    }

    if (current->state == state) {
        unlock_scheduler();
        return current;
    }

    while (current->next != 0) {
        if (current->next->state == state) {
            unlock_scheduler();
            return current->next;
        }
        current = current->next;
    }

    unlock_scheduler();
    return 0;
}

int16_t get_free_pid() {
    lock_scheduler();
    int16_t pid = 1;
    while (get_task(pid) != 0) {
        pid++;
    }
    printf("Free pid: %d\n", pid);
    unlock_scheduler();
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
void go(uint32_t preempt) {

    shithole_task = malloc(sizeof(struct task));
    memset(shithole_task, 0, sizeof(struct task));
    schedule();
    if (preempt) {
        set_preeption_ticks(preempt);
        enable_preemption();
    }
    struct cpu * cpu = get_cpu(current_task->processor);
    tss_set_stack(cpu->tss, (void*)current_task->stack_top, 0);
    ctxswtch(
        shithole_task,
        current_task,
        shithole_task->fxsave_region,
        current_task->fxsave_region
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
    lock_scheduler();
    printf("Exiting task %d\n", current_task->pid);
    current_task->exit_code = 0;
    current_task->state = TASK_ZOMBIE;
    unlock_scheduler();
    yield();
}

void kill_task(int16_t pid) {
    lock_scheduler();
    struct task* task = get_task(pid);
    if (task == 0) {
        printf("No such task\n");
        unlock_scheduler();
        return;
    }

    printf("Killing task %d\n", pid);
    task->exit_code = 0;
    task->exit_signal = SIGKILL;
    task->state = TASK_ZOMBIE;
    unlock_scheduler();
}

void atask() {
    int i = 10;
    while (1) {
        printf("atask: %d\n", i++);
        yield();
    }
    exit();
}

void btask() {
    int i = 20;
    while (1) {
        printf("btask: %d\n", i++);
        yield();
    }
    exit();
}

void subscribe_signal(int signal, sighandler_t handler) {
    lock_scheduler();
    current_task->signal_handlers[signal] = handler;
    unlock_scheduler();
}

void add_signal(int16_t pid, int signal, void * data, uint64_t size) {
    lock_scheduler();
    struct task* task = get_task(pid);
    if (task == 0) {
        printf("No such task\n");
        unlock_scheduler();
        return;
    }

    //Add signal to signal_queue linked list
    struct task_signal * signal_struct = malloc(sizeof(struct task_signal));
    signal_struct->signal = signal;
    signal_struct->next = 0;
    signal_struct->signal_data = data;
    signal_struct->signal_data_size = size;

    if (task->signal_queue == 0) {
        task->signal_queue = signal_struct;
    } else {
        struct task_signal * current = task->signal_queue;
        while (current->next != 0) {
            current = current->next;
        }
        current->next = signal_struct;
    }
    
    unlock_scheduler();
}

void returnoexit() {
    panic("Returned from a process!\n");
}

struct task* create_task(void * init_func, const char * tty) {
    lock_scheduler();

    struct task * task = malloc(sizeof(struct task));
    task->state = TASK_READY;
    task->sigpending = 0;
    task->nice = 0;
    task->mm = 0;
    task->frame = 0;
    task->processor = get_cpu_index();
    task->cpu_time = 0;
    task->last_scheduled = 0;
    task->sleep_time = 0;
    task->exit_code = 0;    
    task->exit_signal = 0;
    task->pdeath_signal = 0;
    task->signal_queue = 0;
    memset(task->signal_handlers, 0, sizeof(sighandler_t) * TASK_SIGNAL_MAX);
    memset(task->fxsave_region, 0, 512);
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
    task->open_files = malloc(sizeof(int) * 32);
    memset(task->open_files, -1, sizeof(int) * 32);
    if (task->open_files == 0) {
        printf("Failed to allocate open_files");
    } else {
        //Check if tty is valid
        if (tty != 0) {
            struct device* dev = device_search(tty);
            if (dev != 0) {
                char buf[256] = {0};
                strcpy(buf, tty);
                strcat(buf, "p0/");
                task->open_files[0] = vfs_file_open(buf, 0, 0);
                if (task->open_files[0] == -1) {
                    printf("Failed to open tty %s\n", buf);
                } else {
                    memcpy(&(task->open_files[1]), &(task->open_files[0]), sizeof(int));
                    memcpy(&(task->open_files[2]), &(task->open_files[0]), sizeof(int));
                }
            }
        }
    }

    task->entry = init_func;
    task->stack = (uint64_t)stackalloc(STACK_SIZE);
    task->stack_top = task->stack + STACK_SIZE;

    //Check stack alignment
    if (task->stack_top % 16 != 0) {
        panic("Stack is not aligned\n");
    }

    ctxcreat(&(task->stack_top), init_func, task->fxsave_region);

    task->pd = duplicate_current_pml4();
    strncpy(task->tty, tty, strlen(tty));
    task->descriptors = 0;
    task->next = 0;
    task->prev = 0;

    printf("Created task with pid %d for address %p\n", task->pid, init_func);
    unlock_scheduler();
    return task;
}

void pause_task(struct task* task) {
    lock_scheduler();
    //printf("Pausing task %d\n", task->pid);
    task->state = TASK_UNINTERRUPTIBLE;
    unlock_scheduler();
}
void resume_task(struct task* task) {
    lock_scheduler();
    //printf("Resuming task %d\n", task->pid);
    task->state = TASK_READY;
    unlock_scheduler();
}

void init_scheduler() {
    printf("### SCHEDULER STARTUP ###\n");
    lock_scheduler();
    struct task * idle_task = create_task(idle, "default"); //Spawn idle task
    idle_task->pid = 0;
    idle_task->state = TASK_IDLE;

    add_task(idle_task);

    current_task = task_head;

    printf("### SCHEDULER STARTUP DONE ###\n");
    unlock_scheduler();
}

void set_current_tty(char * tty) {
    lock_scheduler();
    if (current_task == 0) {
        unlock_scheduler();
        return;
    }
    memset(current_task->tty, 0, 32);
    strncpy(current_task->tty, tty, strlen(tty));
    //Make sure it's null terminated
    current_task->tty[strlen(tty)] = '\0';
    unlock_scheduler();
}

void task_test() {
    printf("Task a is at: %p\n", &atask);
    printf("Task b is at: %p\n", &btask);
}

void reset_current_tty() {
    lock_scheduler();
    if (current_task == 0) {
        unlock_scheduler();
        return;
    }
    memset(current_task->tty, 0, 32);
    strncpy(current_task->tty, "default\0", 8);
    unlock_scheduler();
}

void process_signals() {
    lock_scheduler();
    //Process all signals in the queue
    if (current_task->signal_queue != 0) {
        struct task_signal * current = current_task->signal_queue;
        while (current != 0) {
            if (current_task->signal_handlers[current->signal] != 0) {
                current_task->signal_handlers[current->signal](current->signal, current->signal_data, current->signal_data_size);
            }
            struct task_signal * next = current->next;
            free(current);
            current = next;
        }
        current_task->signal_queue = 0;
    }
    unlock_scheduler();
}