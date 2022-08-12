#include "bootservices/bootservices.h"

#include "util/dbgprinter.h"
#include "memory/memory.h"
#include "io/interrupts.h"

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}

void _start(void) {

    const char * name = get_bootloader_name();
    dbg_print("Bootloader: ");
    dbg_print(name);
    dbg_print("\nTotal Memory: 0x");
    dbg_print(itoa(get_total_memory(), 16));
    dbg_print("\nFree Memory:  0x");
    dbg_print(itoa(get_free_memory(), 16));
    dbg_print("\n");
    
    init_memory();
    init_interrupts();

    uint64_t * ptr = (uint64_t*)0x0000ffffffff;
    *ptr = 0x1234567812345678;
    dbg_print("Im still alive, gonna sleep for a while\n");

    done();
}