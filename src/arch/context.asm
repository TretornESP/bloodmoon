[bits 64]
getContext:
    pop QWORD [rdi + 128]
    pop QWORD [rdi]
    pop QWORD [rdi + 40]

    mov [rdi + 8], rbx
    mov [rdi + 16], rcx
    mov [rdi + 24], rdx
    mov [rdi + 32], rsi
    mov [rdi + 48], r8
    mov [rdi + 56], r9
    mov [rdi + 64], r10
    mov [rdi + 72], r11
    mov [rdi + 80], r12
    mov [rdi + 88], r13
    mov [rdi + 96], r14
    mov [rdi + 104], r15
    mov [rdi + 112], rsp
    mov [rdi + 120], rbp

    push QWORD [rdi + 128]
    ret

setContext:
    mov rax, [rdi]
    mov rbx, [rdi+8]
    mov rcx, [rdi+16]
    mov rdx, [rdi+24]
    mov rsi, [rdi+32]
    mov r8,  [rdi+48]
    mov r9,  [rdi+56]
    mov r10, [rdi+64]
    mov r11, [rdi+72]
    mov r12, [rdi+80]
    mov r13, [rdi+88]
    mov r14, [rdi+96]
    mov r15, [rdi+104]
    mov rsp, [rdi+120]
    mov rbp, [rdi+120]
    mov rdi, [rdi+40]
    pop rbp
    ret

GLOBAL getContext
GLOBAL setContext