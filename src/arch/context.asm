[bits 64]

global save_context

; void update_task();
extern update_taskS

; void save_context(struct task* task);
; rsp in task + 0x8
save_context:
    cli
    pushq rbx
    pushq r12
    pushq r13
    pushq r14

    movq rax, [rdi + 0x8]
    movq [rax], rsp

    ; call update_task
    movq rdi, rax
    call update_task

    