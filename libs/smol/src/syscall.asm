global _syscall_write, _syscall_read, _syscall_open, _syscall_close

_syscall_read:
    mov rax, 0
    syscall
    ret

_syscall_write:
    mov rax, 1
    syscall
    ret

_syscall_open:
    mov rax, 2
    syscall
    ret

_syscall_close:
    mov rax, 3
    syscall
    ret
