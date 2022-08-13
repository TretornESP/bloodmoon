#include "bootservices/bootservices.h"

#include "util/dbgprinter.h"
#include "util/printf.h"
#include "memory/memory.h"
#include "memory/paging.h"
#include "io/interrupts.h"
#include "util/string.h"

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

    void * target = request_page();
    memset(target, 0x0, 0x1000);
    map_memory((void*)0xffffffffdeadb000, target);

    uint64_t* ptr1 = (uint64_t*)0xffffffffdeadb000;

    PAGE_RESTRICT_WRITE((void*)ptr1);

    *ptr1 = 0xdeadbeef;

    printf("Im still alive, gonna sleep for a while\n");

    done();
}