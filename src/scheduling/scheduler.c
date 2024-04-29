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

struct task *task_head[SCHED_PRIORITIES] = {0};
struct task * current_task = 0;
struct task * shithole_task = 0;
struct task * idle_task = 0;

//Returns the task that must enter cpu

struct task * find_valid_task(struct task * head) {
    struct task * current = head;
    while (current != 0) {
        if (current->state == TASK_READY || current->state == TASK_EXECUTING) {
            return current;
        }
        current = current->next;
    }
    return 0;
}

long find_task_prio(struct task * item) {
    for (int i = 0; i < SCHED_PRIORITIES; i++) {
        struct task * current = task_head[i];
        while (current != 0) {
            if (current == item) {
                return i;
            }
            current = current->next;
        }
    }
}

void dump_task_queues() {
    for (int i = 0; i < SCHED_PRIORITIES; i++) {
        struct task * current = task_head[i];
        while (current != 0) {
            printf("Task %d, prio %d, addr %llx\n", current->pid, i, current->entry);
            current = current->next;
        }
    }

}

void change_priority_queue(struct task * proc, long nice) {
    lock_scheduler();
    if (proc->current_nice == nice) {
        unlock_scheduler();
        return;
    }

    struct task * new_queue = task_head[nice];

    if (proc->prev) {
        if (proc->next) {
            proc->prev->next = proc->next;
            proc->next->prev = proc->prev;
        } else {
            proc->prev->next = 0;
        
        }
    } else {
        if (proc->next) {
            proc->next->prev = 0;
        }
    }

    if (new_queue == 0) {
        task_head[nice] = proc;
        proc->next = 0;
        proc->prev = 0;
    } else {
        struct task * current = new_queue;
        while (current->next != 0) {
            current = current->next;
        }
        current->next = proc;
        proc->prev = current;
        proc->next = 0;
    }
    unlock_scheduler();
}

void reset_priority_queue(struct task * proc) {
    change_priority_queue(proc, proc->nice);
}

void detect_age_requirement() {

    unsigned long long time = get_ticks_since_boot();

    for (int i = 0; i < SCHED_PRIORITIES; i++) {
        struct task * current = task_head[i];
        while (current != 0 && current->current_nice > 0) {
            if (current->state == TASK_READY || current->state == TASK_EXECUTING) {
                if ((time - current->last_scheduled) > SCHED_AGE_THRESHOLD) {
                    //dbg_print("Aging task ");
                    //dbg_print(itoa(current->pid, 10));
                    //dbg_print(" from ");
                    //dbg_print(itoa(current->current_nice, 10));
                    //dbg_print(" to ");
                    //dbg_print(itoa(current->current_nice - 1, 10));
                    change_priority_queue(current, current->current_nice - 1);
                }
            }
            current = current->next;
        }
    }
    
}

struct task * schedule() {

    struct task* original = current_task;
    struct task * next_task = 0;
    
    for (int i = 0 ;i < SCHED_PRIORITIES; i++) {
        struct task* current = task_head[i];
        while (current != 0 && current != original) {
            if (current->state == TASK_READY || current->state == TASK_EXECUTING) {
                next_task = current;
                break;
            }
            current = current->next;
        }
    }

    if (next_task == 0) {
        //If we can run the original task, do so
        if (original->state == TASK_READY || original->state == TASK_EXECUTING) {
                next_task = original;
        } else {
            if (idle_task == 0)
                panic("Idle task is not valid\n");
            //Else, run the idle task
            next_task = idle_task;
            if (next_task->state != TASK_IDLE)
                panic("Idle task is not valid\n");
        }
    }

    struct task * prev_task = current_task;  

    if (current_task->state == TASK_EXECUTING)  
        current_task->state = TASK_READY;
    
    current_task = next_task;

    if (current_task->state == TASK_READY)
        current_task->state = TASK_EXECUTING;

    //printf("Switching from task %d to %d\n", prev_task->pid, current_task->pid);

    //dbg_print("Switching to ");
    //dbg_print(itoa(next_task->pid, 10));
    //dbg_print(" from ");
    //dbg_print(itoa(prev_task->pid, 10));
    //dbg_print("\n");
    return prev_task;
}

void block_task(long reason) {
    printf("Blocking task %d\n", current_task->pid);
    lock_scheduler_no_cli();
    current_task->state = reason;
    yield();
    unlock_scheduler_no_sti();
}

void unblock_task(struct task * task) {
    printf("Unblocking task %d\n", task->pid);
    lock_scheduler_no_cli();
    task->state = TASK_READY;
    yield();
    unlock_scheduler_no_sti();
}

void __attribute__((noinline)) yield() {
    if (irq_disable_counter) {
        return;
    } else {
        lock_scheduler_no_cli();
    }

    if (current_task->last_scheduled != 0)
        current_task->cpu_time += (get_ticks_since_boot() - current_task->last_scheduled);

    //reset_priority_queue(current_task);
    //detect_age_requirement();
    struct task * prev = schedule();
    //printf("Switching from task %d to %d\n", prev->pid, current_task->pid);

    current_task->last_scheduled = get_ticks_since_boot();

    struct cpu * cpu = get_cpu(current_task->processor);
    tss_set_stack(cpu->tss, (void*)current_task->stack_top, 0);
    process_signals();
    __asm__ volatile("cli");
    unlock_scheduler_no_sti();
    ctxswtch(
        prev,
        current_task,
        prev->fxsave_region,
        current_task->fxsave_region
    );
}

void add_task(struct task* task) {
    lock_scheduler();
    printf("Adding task %d\n", task->pid);

    if (task_head[task->current_nice] == 0) {
        task_head[task->current_nice] = task;
        task->next = 0;
        task->prev = 0;
        unlock_scheduler();
        return;
    }

    if (task_head[task->current_nice] == 0) {
        task_head[task->current_nice] = task;
        task->next = 0;
        task->prev = 0;
    } else {
        struct task * current = task_head[task->current_nice];
        while (current->next != 0) {
            current = current->next;
        }
        current->next = task;
        task->prev = current;
        task->next = 0;
    }

    unlock_scheduler();
}

void remove_task(struct task* task) {
    lock_scheduler();
    if (task->prev == 0) {
        task_head[task->current_nice] = task->next;
        if (task_head[task->current_nice] != 0) {
            task_head[task->current_nice]->prev = 0;
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
    lock_scheduler_no_cli();
    for (int i = 0; i < SCHED_PRIORITIES; i++) {
        struct task* current = task_head[i];
        if (current == 0) {
            continue;
        }

        if (current->pid == pid) {
            unlock_scheduler_no_sti();
            return current;
        }

        while (current->next != 0) {
            if (current->next->pid == pid) {
                unlock_scheduler_no_sti();
                return current->next;
            }
            current = current->next;
        }
    }

    unlock_scheduler_no_sti();
    return 0;
}

struct task* get_status(long state) {
    lock_scheduler_no_cli();

    for (int i = 0; i < SCHED_PRIORITIES; i++) {
        struct task* current = task_head[i];
        if (current == 0) {
            unlock_scheduler_no_sti();
            return 0;
        }

        if (current->state == state) {
            unlock_scheduler_no_sti();
            return current;
        }

        while (current->next != 0) {
            if (current->next->state == state) {
                unlock_scheduler_no_sti();
                return current->next;
            }
            current = current->next;
        }
    }

    unlock_scheduler_no_sti();
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
    for (int i = 0; i < SCHED_PRIORITIES; i++) {
        struct task* current = task_head[i];
        if (current == 0) {
            continue;
        }

        while (current != 0) {
            printf("PID: %d PPID: %d STATE: %d FLAGS: %d, ENTRY: %p TTY: %s\n", current->pid, current->ppid, current->state, current->flags, current->entry, current->tty);
            current = current->next;
        }
    }

    return;
}

struct task* get_current_task() {   
    return current_task;
}

//void go(uint8_t preempt) {
void go(uint32_t preempt) {
    __asm__ volatile("cli");
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
        printf("kill No such task\n");
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
    lock_scheduler_no_cli();
    current_task->signal_handlers[signal] = handler;
    unlock_scheduler_no_sti();
}

void add_signal(int16_t pid, int signal, void * data, uint64_t size) {
    lock_scheduler();
    struct task* task = get_task(pid);
    if (task == 0) {
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
            if (current->next == current) 
                panic("Signal loop detected\n");
            current = current->next;
        }
        current->next = signal_struct;
    }
    
    unlock_scheduler();
}

void returnoexit() {
    panic("Returned from a process!\n");
}

struct task* create_task(void * init_func, long nice, const char * tty) {
    lock_scheduler();

    //Priority 0 is used to force scheduling for aged tasks
    if (nice > SCHED_PRIORITIES || nice == 0) {unlock_scheduler(); return 0;}

    struct task * task = malloc(sizeof(struct task));
    task->state = TASK_READY;
    task->sigpending = 0;
    task->nice = nice;
    task->current_nice = nice;
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
    task->stack_top = task->stack + STACK_SIZE - 0x8; //WARNING: -0x8 is a hack to make sure the stack is aligned

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
    lock_scheduler_no_cli();
    //printf("Pausing task %d\n", task->pid);
    task->state = TASK_UNINTERRUPTIBLE;
    unlock_scheduler_no_sti();
}
void resume_task(struct task* task) {
    lock_scheduler_no_cli();
    //printf("Resuming task %d\n", task->pid);
    task->state = TASK_READY;
    unlock_scheduler_no_sti();
}

void init_scheduler() {
    printf("### SCHEDULER STARTUP ###\n");
    lock_scheduler();
    idle_task = create_task(idle, 3, "default"); //Spawn idle task
    idle_task->pid = 0;
    idle_task->state = TASK_IDLE;

    add_task(idle_task);

    current_task = task_head[4];

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
    //Process one signal in the queue
    if (current_task->signal_queue != 0) {
        struct task_signal * current = current_task->signal_queue;
        if (current->next != 0) {
            current_task->signal_queue = current->next;
        } else {
            current_task->signal_queue = 0;
        }

        sighandler_t handler = current_task->signal_handlers[current->signal];
        free(current);

        if (handler != 0) {
            handler(current->signal, current->signal_data, current->signal_data_size);
        }
    }
    unlock_scheduler();
}