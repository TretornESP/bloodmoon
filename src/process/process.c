#include "process.h"
#include "../util/printf.h"
#include "../util/string.h"
#include "../memory/paging.h"

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

CPU_CONTEXT ctx[2] = {0};
uint8_t ctxi = 0;

void t1();
void t2();
void ksched();

void kyield() {
    //Kernel stuff
    ksched();
}

void kwrite(const char* str) {
    printf("%s", str);
    kyield();
}

void t1() {
    int i = 10;
    while (i--) {
        kwrite("a");
    }
}

void t2() {
    int i = 10;
    while (i--) {
        kwrite("b");
    }
}

void ksched() {
    CPU_CONTEXT *current = &ctx[ctxi];
    CPU_CONTEXT *next = &ctx[!ctxi];
    ctxi = !ctxi;
    swap_context(current, next);
}

void swap_test() {
    printf("Starting swap test\n");
    //ctx[0].rsp = (uint64_t) stack_alloc(4096);
    //ctx[0].rsp += 4096;
    //ctx[0].rsp -= sizeof(CPU_CONTEXT);
    //ctx[0].rbp = ctx[0].rsp;
    //ctx[0].rsp -= 8;
    //ctx[0].rip = (uint64_t) t1;
    //
    //ctx[1].rsp = (uint64_t) stack_alloc(4096);
    //ctx[1].rsp += 4096;
    //ctx[1].rsp -= sizeof(CPU_CONTEXT);
    //ctx[1].rbp = ctx[1].rsp;
    //ctx[1].rsp -= 8;
    //ctx[1].rip = (uint64_t) t2;
    
    ksched();
}