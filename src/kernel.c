#include "bootservices/bootservices.h"

#include "util/dbgprinter.h"
#include "util/printf.h"
#include "memory/memory.h"
#include "memory/paging.h"
#include "io/interrupts.h"
#include "util/string.h"
#include "memory/heap.h"
#include "drivers/keyboard.h"
#include "dev/acpi.h"

void _start(void) {

    printf("Bootloader: %s\n", get_bootloader_name());
    printf("Total Memory: 0x%llx\n", get_total_memory());
    printf("Free Memory:  0x%llx\n", get_free_memory());
    printf("Kernel physical address: 0x%llx\n", get_kernel_address_physical());
    printf("Kernel virtual address:  0x%llx\n", get_kernel_address_virtual());
    printf("KERNEL START: %llx\n", &KERNEL_START);
    printf("KERNEL END:   %llx\n", &KERNEL_END);

    init_memory();
    init_paging();
    init_heap();
    init_interrupts();

    debug_acpi();
    while(1);
}