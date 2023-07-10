[bits 64]
save_context:
    mov [rdi], rax
    mov [rdi+8], rbx
    mov [rdi+16], rcx
    mov [rdi+24], rdx
    mov [rdi+32], rsi
    mov [rdi+40], rdi
    mov [rdi+48], r8
    mov [rdi+56], r9
    mov [rdi+64], r10
    mov [rdi+72], r11
    mov [rdi+80], r12
    mov [rdi+88], r13
    mov [rdi+96], r14
    mov [rdi+104], r15
    mov [rdi+112], rsp
    mov [rdi+120], rbp
    mov [rdi+128], rax
    ret

; rdi source context
; rsi destination context
; Both type CPU_CONTEXT
; typedef struct __attribute__((packed)){
;     uint64_t rax;
;     uint64_t rbx;
;     uint64_t rcx;
;     uint64_t rdx;
;     uint64_t rsi;
;     uint64_t rdi;
;     uint64_t r8;
;     uint64_t r9;
;     uint64_t r10;
;     uint64_t r11;
;     uint64_t r12;
;     uint64_t r13;
;     uint64_t r14;
;     uint64_t r15;
;     uint64_t rsp;
;     uint64_t rbp;
;     uint64_t rip;
; } CPU_CONTEXT ;

swap_context:
    mov [rdi], rax
    mov [rdi+8], rbx
    mov [rdi+16], rcx
    mov [rdi+24], rdx
    mov [rdi+32], rsi
    mov [rdi+40], rdi
    mov [rdi+48], r8
    mov [rdi+56], r9
    mov [rdi+64], r10
    mov [rdi+72], r11
    mov [rdi+80], r12
    mov [rdi+88], r13
    mov [rdi+96], r14
    mov [rdi+104], r15
    mov [rdi+112], rsp
    mov [rdi+120], rbp
    mov [rdi+128], rax
    mov rax, [rsi]
    mov rbx, [rsi+8]
    mov rcx, [rsi+16]
    mov rdx, [rsi+24]
    mov rsi, [rsi+32]
    mov rdi, [rsi+40]
    mov r8,  [rsi+48]
    mov r9,  [rsi+56]
    mov r10, [rsi+64]
    mov r11, [rsi+72]
    mov r12, [rsi+80]
    mov r13, [rsi+88]
    mov r14, [rsi+96]
    mov r15, [rsi+104]
    mov rsp, [rsi+112]
    mov rbp, [rsi+120]
    mov rsi, [rsi+128]
    ret

GLOBAL save_context
GLOBAL swap_context