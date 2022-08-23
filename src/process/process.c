#include "process.h"
#include "../util/printf.h"

void dump_context(CPU_CONTEXT* ctx) {
    printf(
        "Context {  RAX: 0x%llx RBX: 0x%llx RCX: 0x%llx RDX: 0x%llx\n"\
        "           RSI: 0x%llx RDI: 0x%llx r8: 0x%llx r9: 0x%llx\n"\
        "           r10: 0x%llx r11: 0x%llx r12: 0x%llx r13: 0x%llx\n"\
        "           r14: 0x%llx r15: 0x%llx RBP: 0x%llx RSP: 0x%llx\n"\
        "           RIP: 0x%llx}\n"\
        "}\n",
        ctx->rax, ctx->rbx, ctx->rcx, ctx->rdx,
        ctx->rsi, ctx->rdi, ctx->r8, ctx->r9,
        ctx->r10, ctx->r11, ctx->r12, ctx->r13,
        ctx->r14, ctx->r15, ctx->rbp, ctx->rsp,
        ctx->rip
    );    
}