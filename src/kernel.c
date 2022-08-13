#include "bootservices/bootservices.h"

#include "util/dbgprinter.h"
#include "util/printf.h"
#include "memory/memory.h"
#include "memory/paging.h"
#include "io/interrupts.h"
#include "util/string.h"
#include "memory/heap.h"

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}

void _start(void) {

    printf("Bootloader: %s\n", get_bootloader_name());
    printf("Total Memory: 0x%llx\n", get_total_memory());
    printf("Free Memory:  0x%llx\n", get_free_memory());
    printf("Kernel physical address: 0x%llx\n", get_kernel_address_physical());
    printf("Kernel virtual address:  0x%llx\n", get_kernel_address_virtual());
    printf("KERNEL START: %llx\n", &KERNEL_START);
    printf("KERNEL END:   %llx\n", &KERNEL_END);

    init_memory();
    init_interrupts();
    init_paging();
    init_heap((void*)0x0000100000000000, 0x10);

    for (int i = 0; i < 10; i++)
        printf("Malloced address: 0x%llx\n", (uint64_t)malloc(0x100));

    printf("Im still alive, gonna sleep for a while\n");

    done();
}