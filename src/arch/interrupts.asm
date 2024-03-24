extern interrup_handler

.macro swapgs_if_necessary
	cmp $0x08, 0x8(%rsp)
	je 1f
	swapgs
1:
.endm

interrupt_entry:
    swapgs_if_necessary
    push %r15
    push %r14
    ...
    pop %r14
    pop %r15
    swapgs_if_necessary
    iretq