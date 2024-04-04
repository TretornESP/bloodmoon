[bits 64]
global ctxswtch
global ctxsave

; Inputs:
;struct cpu_context {
;    uint64_t cr3;
;    
;    uint64_t rax;
;    uint64_t rbx;
;    uint64_t rcx;
;    uint64_t rdx;
;    uint64_t r8;
;    uint64_t r9;
;    uint64_t r10;
;    uint64_t r11;
;    uint64_t r12;
;    uint64_t r13;
;    uint64_t r14;
;    uint64_t r15;
;
;    uint64_t rsi;  
;    uint64_t rdi;
;    uint64_t rbp;
;    uint64_t rsp; 
;
;    uint64_t interrupt_number; 
;    uint64_t error_code; 
;    
;    uint64_t rip; 
;    uint64_t rflags; 
;
;    uint64_t cs; 
;    uint64_t ss;
;    uint64_t gs;
;    uint64_t fs;
;}

ctxsave:
    cli

    ; Save all registers
    push rax
    mov rax, cr3
    mov qword [rdi], rax
    pop rax
    mov qword [rdi + 8], rax
    mov qword [rdi + 16], rbx
    mov qword [rdi + 24], rcx
    mov qword [rdi + 32], rdx
    mov qword [rdi + 40], r8
    mov qword [rdi + 48], r9
    mov qword [rdi + 56], r10
    mov qword [rdi + 64], r11
    mov qword [rdi + 72], r12
    mov qword [rdi + 80], r13
    mov qword [rdi + 88], r14
    mov qword [rdi + 96], r15
    mov qword [rdi + 104], rsi
    mov qword [rdi + 112], rdi
    mov qword [rdi + 120], rbp
    mov qword [rdi + 128], rsp
    ;mov dummy values for interrupt_number and error_code
    mov qword [rdi + 136], 0
    mov qword [rdi + 144], 0
    ;mov dummy values for rip
    mov qword [rdi + 152], 0
    ;save rflags
    push rax
    pushfq
    pop rax
    mov qword [rdi + 160], rax
    ;save segment registers
    mov rax, cs
    mov qword [rdi + 168], rax
    mov rax, ss
    mov qword [rdi + 176], rax
    mov rax, gs
    mov qword [rdi + 184], rax
    mov rax, fs
    mov qword [rdi + 192], rax
    pop rax
    ret

; Inputs:
; RDI: struct task* old
; RSI: struct task* new

ctxswtch:
    cli

    ; Push registers rbx, r12, r13, r14, r15
    push rbp
    push rax
    push rbx
    push rcx
    push rdx
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    push rsi
    push rdi

    ; Save stack pointer in old's rsp_top
    mov qword [rdi + 8], rsp
    
    ; Load new's pd into CR3
    mov rax, [rsi + 16]
    mov cr3, rax

    ; Load rsp with new's rsp_top
    mov rsp, qword [rsi + 8]

    ; Pop registers r15, r14, r13, r12, rbx
    pop rdi
    pop rsi
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp

    sti
    ret
