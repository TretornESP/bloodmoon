#include "process.h"
#include "../util/printf.h"
#include "../util/string.h"
#include "../memory/paging.h"
#include "../memory/heap.h"

void dump_context(CPU_CONTEXT* ctx) {
    printf(
        "Context {  RAX: 0x%llx RBX: 0x%llx RCX: 0x%llx RDX: 0x%llx\n"\
        "           r8: 0x%llx r9: 0x%llx\n"\
        "           r10: 0x%llx r11: 0x%llx r12: 0x%llx r13: 0x%llx\n"\
        "           r14: 0x%llx r15: 0x%llx RBP: 0x%llx RSP: 0x%llx\n}\n"\
        "}\n",
        ctx->rax, ctx->rbx, ctx->rcx, ctx->rdx,
        ctx->r8, ctx->r9,
        ctx->r10, ctx->r11, ctx->r12, ctx->r13,
        ctx->r14, ctx->r15, ctx->rbp, ctx->rsp
    );    
}

CPU_CONTEXT* allocate_process(void* init) {
    CPU_CONTEXT * context = malloc(sizeof(CPU_CONTEXT));
    memset(context, 0, sizeof(CPU_CONTEXT));

    context->rsp = (uint64_t)stackalloc(STACK_SIZE);
    context->rbp = context->rsp;

    struct page_directory* pd = get_pml4();
    uint64_t physaddr = virtual_to_physical(pd, (void*)context->rsp);

    printf("Process stack allocated at 0x%llx\n", physaddr);
    //Set up the stack, idk if this is correct
    uint64_t* stack = (uint64_t*)context->rsp;
    stack[0] = 0x10; //SS
    stack[1] = context->rsp; //RSP
    stack[2] = 0x200; //RFLAGS
    stack[3] = 0x08; //CS
    stack[4] = (uint64_t)init; //RIP

    context->rsp -= sizeof(uint64_t) * 5;
    return context;
}