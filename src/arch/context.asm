[bits 64]

; Both type CPU_CONTEXT
; typedef struct __attribute__((packed)){
;     uint64_t rax;
;     uint64_t rbx;
;     uint64_t rcx;
;     uint64_t rdx;
;     uint64_t r8;
;     uint64_t r9;
;     uint64_t r10;
;     uint64_t r11;
;     uint64_t r12;
;     uint64_t r13;
;     uint64_t r14;
;     uint64_t r15;
;     uint64_t rbp;
;     uint64_t rsp;
; } CPU_CONTEXT ;   

; void swap_context(CPU_CONTEXT *current, CPU_CONTEXT *next);
swap_context:
    mov [rdi + 0], rax
    mov [rdi + 8], rbx
    mov [rdi + 16], rcx
    mov [rdi + 24], rdx
    mov [rdi + 32], r8
    mov [rdi + 40], r9
    mov [rdi + 48], r10
    mov [rdi + 56], r11
    mov [rdi + 64], r12
    mov [rdi + 72], r13
    mov [rdi + 80], r14
    mov [rdi + 88], r15
    mov [rdi + 96], rbp
    mov [rdi + 104], rsp
    mov rax, [rsi + 0]
    mov rbx, [rsi + 8]
    mov rcx, [rsi + 16]
    mov rdx, [rsi + 24]
    mov r8, [rsi + 32]
    mov r9, [rsi + 40]
    mov r10, [rsi + 48]
    mov r11, [rsi + 56]
    mov r12, [rsi + 64]
    mov r13, [rsi + 72]
    mov r14, [rsi + 80]
    mov r15, [rsi + 88]
    mov rbp, [rsi + 96]
    mov rsp, [rsi + 104]
    ret

GLOBAL swap_context
