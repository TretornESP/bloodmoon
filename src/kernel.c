#include "bootservices/bootservices.h"

#include "util/dbgprinter.h"
#include "util/string.h"
#include "memory/memory.h"
#include "test/allocatest.h"

static void done(void) {
    for (;;) {
        __asm__("hlt");
    }
}

void _start(void) {

    allocatest();
/*
    const char * name = get_bootloader_name();
    dbg_print("Bootloader: ");
    dbg_print(name);
    dbg_print("\nTotal Memory: 0x");
    dbg_print(itoa(get_total_memory(), 16));
    dbg_print("\nFree Memory:  0x");
    dbg_print(itoa(get_free_memory(), 16));
    dbg_print("\n");
    
    init_memory();

    void * cleanup_list[0x5];

    for (int i = 0; i < 0x5; i++) {
        cleanup_list[i] = request_page();
        memset(cleanup_list[i], 0xff, 0x1000);
        dbg_print("Requesting page: 0x");
        dbg_print(itoa((uint64_t)cleanup_list[i], 16));
        dbg_print(" content [");
        for (int j = 0; j < 0x10; j++) {
            dbg_print(itoa(((uint8_t*)cleanup_list[i])[j], 16));
        }
        dbg_print("]\n");
    }
    for (int i = 0; i < 0x1faf6; i++) {
        void * addr = request_page();
        memset(addr, 0xFF, 0x1000);
    }
    for (int i = 0; i < 0x5; i++) {
        free_page(cleanup_list[i]);
    }
    for (int i = 0; i < 0x5; i++) {
        cleanup_list[i] = request_page();
        memset(cleanup_list[i], 0xff, 0x1000);
        dbg_print("Requesting page: 0x");
        dbg_print(itoa((uint64_t)cleanup_list[i], 16));
        dbg_print(" content [");
        for (int j = 0; j < 0x10; j++) {
            dbg_print(itoa(((uint8_t*)cleanup_list[i])[j], 16));
        }
        dbg_print("]\n");
    }
*/
    done();
}