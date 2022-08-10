#include "bootservices/bootservices.h"

#include "util/dbgprinter.h"
#include "memory/memory.h"
static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}

void _start(void) {
    const char * name = get_bootloader_name();
    dbg_print("Bootloader: ");
    dbg_print(name);
    dbg_print("\nTotal Memory: ");
    dbg_print(itoa(get_total_memory(), 16));
    dbg_print("\nFree Memory: ");
    dbg_print(itoa(get_free_memory(), 16));
    done();
}