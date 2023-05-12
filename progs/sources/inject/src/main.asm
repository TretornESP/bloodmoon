;Create a simple asm code that loops infinitely without doing nothing else.
;The code must be as small as possible.
[bits 64]
    mov rax, 1
    mov rdi, 1
    mov rsi, msg
    mov rdx, 13
    int 0x80
loop:
    jmp loop

msg:
    db "Hello, world!", 10