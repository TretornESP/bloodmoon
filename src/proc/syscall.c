#include "syscall.h"
#include "../arch/msr.h"
#include "../arch/gdt.h"
#include "../util/printf.h"
#include "../vfs/vfs_interface.h"
#include "../sched/scheduler.h"

#define SYSRET(ctx, val) ctx->rax = val; return;
#define SYSCALL_ARG0(ctx) ctx->rdi
#define SYSCALL_ARG1(ctx) ctx->rsi
#define SYSCALL_ARG2(ctx) ctx->rdx
#define SYSCALL_ARG3(ctx) ctx->rcx

extern void syscall_entry();
void undefined_syscall_handler(struct task* task, struct cpu_context* ctx) {
    (void)task;
    printf("Undefined syscall %d\n", ctx->rax);
    SYSRET(ctx, SYSCALL_UNDEFINED);
}

void read_syscall_handler(struct task* task, struct cpu_context* ctx) {
    if (task->open_files[SYSCALL_ARG0(ctx)] > -1) {
        vfs_file_read(task->open_files[SYSCALL_ARG0(ctx)], (void*)SYSCALL_ARG1(ctx), SYSCALL_ARG2(ctx));
        SYSRET(ctx, SYSCALL_SUCCESS);
    } else {
        printf("[SYSCALL READ] File descriptor %d not open\n", SYSCALL_ARG0(ctx));
        SYSRET(ctx, SYSCALL_ERROR);
    }
}

void write_syscall_handler(struct task* task, struct cpu_context* ctx) {
    if (task->open_files[SYSCALL_ARG0(ctx)] > -1) {
        vfs_file_write(task->open_files[SYSCALL_ARG0(ctx)], (void*)SYSCALL_ARG1(ctx), SYSCALL_ARG2(ctx));
        SYSRET(ctx, SYSCALL_SUCCESS);
    } else {
        printf("[SYSCALL WRITE] File descriptor %d not open\n", SYSCALL_ARG0(ctx));
        SYSRET(ctx, SYSCALL_ERROR);
    }
}

syscall_handler syscall_handlers[SYSCALL_HANDLER_COUNT] = {
    [0] = read_syscall_handler,
    [1] = write_syscall_handler,
    [2 ... SYSCALL_HANDLER_COUNT-1] = undefined_syscall_handler
};

void global_syscall_handler(struct cpu_context* ctx) {

    struct task * current_task = get_current_task();

    if (ctx->rax < SYSCALL_HANDLER_COUNT) {
        syscall_handlers[ctx->rax](current_task, ctx);
    } else {
        printf("Syscall number overflow %d\n", ctx->rax);
        SYSRET(ctx, SYSCALL_ERROR);
    }
}

void syscall_set_user_gs(uintptr_t addr)
{
    cpu_set_msr_u64(MSR_GS_BASE, addr);
}

void syscall_set_kernel_gs(uintptr_t addr)
{
    cpu_set_msr_u64(MSR_KERN_GS_BASE, addr);
}