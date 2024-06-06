#include <elf.h>

#include "scheduler.h"
#include "pit.h"
#include "../cpus/cpus.h"
#include "../memory/heap.h"
#include "../memory/paging.h"
#include "../arch/simd.h"
#include "../arch/gdt.h"
#include "../util/printf.h"
#include "../devices/devices.h"
#include "../util/string.h"
#include "../arch/tss.h"
#include "../arch/simd.h"
#include "../vfs/vfs_interface.h"
#include "../proc/syscall.h"

#define MAX_TASKS 255

atomic_int_t irq_disable_counter = 0;
uint8_t scheduler_ready = 0;

char TTY[] = "default\0";

struct task *task_head = 0;
struct task * current_task = 0;
struct task * boot_task = 0;

//TODO: Implement
//extern void ctxsave(struct cpu_context *ctx);


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
    __asm__("cli");
    if (irq_disable_counter || !scheduler_ready) {
        __asm__("sti");
        return;
    }
    volatile uint64_t eoi = eoi_pending();
    if (eoi) {
        printf("EOI %llx\n", eoi);
        panic("EOI pending\n");
    }
    if (current_task->last_scheduled != 0)
        current_task->cpu_time += (get_ticks_since_boot() - current_task->last_scheduled);

    struct task * prev = schedule();
    current_task->last_scheduled = get_ticks_since_boot();
    process_signals();
    struct cpu * cpu = get_cpu(current_task->processor);
    if (current_task->privilege == KERNEL_TASK) {
        syscall_set_kernel_gs((uint64_t)current_task->context);
        tss_set_stack(cpu->tss, (void*)current_task->stack_top, 0);
        tss_set_stack(cpu->tss, (void*)current_task->alt_stack_top, 3);
    } else {
        syscall_set_user_gs((uint64_t)current_task->context);
        tss_set_stack(cpu->tss, (void*)current_task->stack_top, 3);
        tss_set_stack(cpu->tss, (void*)current_task->alt_stack_top, 0);
    }
    //ctxswtch(prev, current_task, prev->fxsave_region, current_task->fxsave_region);
    newctxswtch(prev->context, current_task->context, prev->fxsave_region, current_task->fxsave_region);
}

void debug_task(struct task* task, uint64_t original_stack_top) {
    //Print stack
    printf("Task %d\n", task->pid);
    printf("State: %d\n", task->state);
    printf("Flags: %d\n", task->flags);
    printf("Privilege: %d\n", task->privilege);
    printf("Entry: %p\n", task->entry);

    printf("------------------------\n");
    uint64_t size = original_stack_top - task->stack_top;
    printf("Stack dump, size: %d\n", size);
    uint64_t * stack = (uint64_t*)task->stack_top;
    for (uint64_t i = 0; i < size; i++) {
        if (i % 8 == 0) {
            printf("\n");
        }
        printf("%02x ", ((uint8_t*)stack)[i]);
    }
    printf("\n------------------------\n");
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

void go(uint32_t preempt) {
    if (scheduler_ready) panic("Scheduler already running\n");
    boot_task = kmalloc(sizeof(struct task));
    memset(boot_task, 0, sizeof(struct task));
    boot_task->context = kmalloc(sizeof(struct cpu_context));
    memset(boot_task->context, 0, sizeof(struct cpu_context));
    boot_task->context->info = kmalloc(sizeof(struct cpu_context_info));
    memset(boot_task->context->info, 0, sizeof(struct cpu_context_info));

    if (preempt) {
        set_preeption_ticks(preempt);
        enable_preemption();
    }

    schedule();
    struct cpu * cpu = get_cpu(current_task->processor);
    syscall_set_kernel_gs((uint64_t)boot_task->context);
    if (current_task->privilege == KERNEL_TASK) {
        syscall_set_kernel_gs((uint64_t)current_task->context);
        tss_set_stack(cpu->tss, (void*)current_task->stack_top, 0);
        tss_set_stack(cpu->tss, (void*)current_task->alt_stack_top, 3);
    } else {
        syscall_set_user_gs((uint64_t)current_task->context);
        tss_set_stack(cpu->tss, (void*)current_task->stack_top, 3);
        tss_set_stack(cpu->tss, (void*)current_task->alt_stack_top, 0);
    }
    scheduler_ready = 1;
    newctxswtch(boot_task->context, current_task->context, boot_task->fxsave_region, current_task->fxsave_region);
}

char * get_current_tty() {
    if (current_task == 0) {
        return TTY;
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
    //Yield
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
    struct task_signal * signal_struct = sigmalloc(sizeof(struct task_signal));
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

//Stack grows downwards
void* push_u64(void * stack, uint64_t value) {
    stack -= 8;
    *(uint64_t*)stack = value;
    return stack;
}

void* push_str(void * stack, char * str) {
    int len = strlen(str);
    stack -= len;
    memcpy(stack, str, len);
    return stack;
}

uint64_t initialize_stack(void * stack, int argc, char* argv[], char *envp[], struct auxv *auxv) {
    int envc = 0;
    if(envp != 0){
        while(envp[envc] != NULL){
            envc++;
        }
    }

    //Stack layout
    //Auxv
    if (auxv != 0) {
        for (uint8_t i = 0; i < 5; i++) {
            if (&auxv[i] == 0) continue;
            printf("Auxv[%d]: %d %p\n", i, auxv[i].a_type, auxv[i].a_val);
            stack = push_u64(stack, auxv[i].a_type);
            stack = push_u64(stack, (uint64_t)auxv[i].a_val);
        }
    }
    //Envp
    for (int i = envc - 1; i >= 0; i--) {
        printf("Envp[%d]: %s\n", i, envp[i]);
        stack = push_str(stack, envp[i]);
    }

    //Allign stack adding nulls
    while ((uint64_t)stack % 16 != 0) {
        *(uint8_t*)stack = 0;
        stack++;
    }

    //Argv
    for (int i = argc - 1; i >= 0; i--) {
        printf("Argv[%d]: %s\n", i, argv[i]);
        stack = push_str(stack, argv[i]);
    }

    //Allign stack adding nulls
    while ((uint64_t)stack % 16 != 0) {
        *(uint8_t*)stack = 0;
        stack++;
    }
    
    //Argc
    stack = push_u64(stack, argc);
    printf("Argc: %d\n", argc);

    return (uint64_t)stack;
}

void initialize_context(struct task* task, void * init_function, int argc, char* argv[], char *envp[], struct auxv *auxv) {
    uint64_t original_stack = task->stack_top;
    task->stack_top = initialize_stack((void*)task->stack_top, argc, argv, envp, auxv);
    if (task->privilege == KERNEL_TASK) {
        newctxcreat(&(task->stack_top), init_function);
    } else {
        newuctxcreat(&(task->stack_top), init_function);
    }

    task->context = kmalloc(sizeof(struct cpu_context));
    memset(task->context, 0, sizeof(struct cpu_context));
    task->context->cr3 = (uint64_t)task->pd;
    task->context->info = kmalloc(sizeof(struct cpu_context_info));
    memset(task->context->info, 0, sizeof(struct cpu_context_info));
    task->context->info->stack = (uint64_t)kstackalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE; //Syscall stack
    task->context->info->cs = task->cs;
    task->context->info->ss = task->ds;
    task->context->info->thread = 0;
    task->context->rax = 2;
    task->context->rbx = 3;
    task->context->rcx = 4;
    task->context->rdx = 5;
    task->context->rsi = 6;
    task->context->rdi = 7;
    task->context->rbp = 8;
    task->context->r8 = 9;
    task->context->r9 = 10;
    task->context->r10 = 11;
    task->context->r11 = 12;
    task->context->r12 = 13;
    task->context->r13 = 14;
    task->context->r14 = 15;
    task->context->r15 = 16;
    task->context->interrupt_number = 0;
    task->context->error_code = 0;
    task->context->rip = (uint64_t)init_function;
    task->context->cs = task->cs;
    task->context->rflags = 0x202;
    task->context->ss = task->ds;

    __asm__ volatile("fxsave %0" : "=m" (task->fxsave_region));

    task->context->rsp = task->stack_top;
    debug_task(task, original_stack);
}

struct page_directory * get_new_pd(uint8_t privilege) {
    struct page_directory * pd = 0;
    if (privilege == KERNEL_TASK) {
        pd = FROM_KERNEL_MAP(duplicate_current_pml4());
    } else {
        pd = FROM_KERNEL_MAP(duplicate_current_kernel_pml4());
    }

    return pd;
}

struct task* create_task(
    void * init_func,
    const char * tty,
    uint8_t privilege,
    int argc,
    char* argv[],
    char* envp[],
    struct auxv *auxv,
    struct page_directory * startup_pd
) {
    lock_scheduler();

    struct task * task = kmalloc(sizeof(struct task));
    task->state = TASK_READY;
    task->flags = 0;
    task->sigpending = 0;
    task->nice = 0;
    task->privilege = privilege;
    task->mm = 0;
    task->frame = 0;
    task->processor = 0; //TODO: Change this for SMP
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
    task->open_files = kmalloc(sizeof(int) * 32);
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
    task->heap = 0x0;
    //Create a stack frame (do not use structs)
    //Set r12-r15 to 0, rbx to 0
    //Set rip to init_func
    if (task->privilege == KERNEL_TASK) {

        if (startup_pd == 0)
            task->pd = FROM_KERNEL_MAP(duplicate_current_pml4());
        else
            task->pd = startup_pd;

        task->stack_base = (uint64_t)kstackalloc(KERNEL_STACK_SIZE);
        task->stack_top = task->stack_base + KERNEL_STACK_SIZE;
        task->cs = get_kernel_code_selector();
        task->ds = get_kernel_data_selector();
        initialize_context(task, init_func, argc, argv, envp, auxv);
    } else {

        if (startup_pd == 0)
            task->pd = FROM_KERNEL_MAP(duplicate_current_kernel_pml4());
        else
            task->pd = startup_pd;

        create_user_heap(task, TO_KERNEL_MAP(request_page()));
        task->stack_base = (uint64_t)ustackalloc(task, USER_STACK_SIZE);
        task->stack_top = task->stack_base + USER_STACK_SIZE;
        task->alt_stack_base = (uint64_t)kstackalloc(KERNEL_STACK_SIZE);
        task->alt_stack_top = task->alt_stack_base + KERNEL_STACK_SIZE;
        task->cs = get_user_code_selector();
        task->ds = get_user_data_selector();
        initialize_context(task, init_func, argc, argv, envp, auxv);
    }

    strncpy(task->regular_tty, tty, strlen(tty));
    strncpy(task->io_tty, get_io_tty(), strlen(get_io_tty()));
    task->tty = task->regular_tty;
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

void returnoexit() {
    panic("Returned from a process!\n");
}

void invstack() {
    panic("Kernel task doesnt have ustack!\n");
}

void init_scheduler() {
    printf("### SCHEDULER STARTUP ###\n");
    lock_scheduler();
    struct task * idle_task = create_task(idle, "default", KERNEL_TASK, 0x0, 0x0, 0x0, 0x0, 0x0); //Spawn idle task
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
    memset(current_task->regular_tty, 0, 32);
    strncpy(current_task->regular_tty, tty, strlen(tty));
    //Make sure it's null terminated
    current_task->regular_tty[strlen(tty)] = '\0';
    unlock_scheduler();
}

void reset_current_tty() {
    lock_scheduler();
    if (current_task == 0) {
        unlock_scheduler();
        return;
    }
    memset(current_task->regular_tty, 0, 32);
    strncpy(current_task->regular_tty, "default\0", 8);
    unlock_scheduler();
}

void __attribute__((noinline)) process_signals() {
    //Process one signal in the queue
    if (current_task->signal_queue != 0) {
        struct task_signal * current = current_task->signal_queue;
        if (current->next != 0) {
            current_task->signal_queue = current->next;
        } else {
            current_task->signal_queue = 0;
        }

        sighandler_t handler = current_task->signal_handlers[current->signal];
        sigfree(current);

        if (handler != 0) {
            handler(current->signal, current->signal_data, current->signal_data_size);
        }
    }
}