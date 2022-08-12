#define MAX_ALLOCS_FOR_512MB 0x1faf6
#include "../util/dbgprinter.h"
#include "../util/string.h"
#include "../memory/memory.h"

void allocatest() {
    if (!init_memory())
        panic("This test should run with fresh memory!");

    dbg_print("Starting tests...\n");

    for (uint8_t t = 0; t < 2; t++) {
        for (uint64_t i = 0; i < MAX_ALLOCS_FOR_512MB; i++) {
            void * addr = request_page();
            memset(addr, 0xFF, 0x1000);
            dbg_print("Bitfield state:");
            for (int i = 0;i < 100; i++) {
                dbg_print(" ");
                dbg_print(itoa(test_get_memory()->bitfield[i], 16));
            }
            dbg_print("\n");
            free_page(addr);
        }
    }

    for (uint64_t i = 6; i < test_get_memory()->total_available_pages; i++) {
        if (test_get_memory()->bitfield[i] != 0) {
            panic("Memory leak detected!");
        }
    }

    dbg_print("Total lock unlock test passed!");    
}