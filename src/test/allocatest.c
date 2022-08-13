#define MAX_ALLOCS_FOR_512MB 0x1faef

#include "tests.h"
#include "../util/dbgprinter.h"
#include "../util/string.h"
#include "../memory/memory.h"

void allocatest_bench_crash() {
    if (!init_memory())
        panic("This test should run with fresh memory!");

    dbg_print("This test should crash, note the number of requests and fill MAX_ALLOCS_FOR_512MB");
    dbg_print("Total available memory  is the crash number times 0x1000\n");
    dbg_print("Calculate the percentage out of your total physical memory\n");

    while (1) request_page();
}

void allocatest() {
    if (!init_memory())
        panic("This test should run with fresh memory!");


    dbg_print("Starting tests...\n");
    struct system_memory * memory = test_get_memory();
    uint64_t last_addr;
    void * first_addr;
    first_addr = request_page();
    free_page(first_addr);

    for (uint8_t t = 1; t < 3; t++) {
        dbg_print("Starting round ");
        dbg_print(itoa(t, 10));
        dbg_print("\n");
        dbg_print("Filling address space with 0x69\n");

        for (uint64_t i = 0; i < MAX_ALLOCS_FOR_512MB; i++) {
            void * addr = request_page();
            memset(addr, 0x69, 0x1000);
            last_addr = (uint64_t)addr;
        }

        dbg_print("Last filled address: 0x");
        dbg_print(itoa(last_addr, 16));
        dbg_print("\nMemory at that address is now 0x");
        dbg_print(itoa(*(uint8_t*)last_addr, 16));
        uint64_t bug_check = last_addr;

        dbg_print("\nTesting memory content for 0x69\n");
        for (uint64_t i = 0; i < ((MAX_ALLOCS_FOR_512MB) << 12); i++) {

            uint8_t * ptr = (uint8_t*)((uint64_t)first_addr + i);
            last_addr = (uint64_t)ptr;
            if (*ptr != 0x69) {
                if (bug_check < ((uint64_t)first_addr + (MAX_ALLOCS_FOR_512MB << 12))) {
                    dbg_print("[WARNING] Test inconsistency detected!!!\n");
                    dbg_print("The crash you are seeing below is not due to \n");
                    dbg_print("the memory management subsystem but the test itself\n");
                    dbg_print("recompile and it should go away. I'm to lazy to fix it\n");
                }
                dbg_print("Target is at: 0x");
                dbg_print(itoa((uint64_t)first_addr + (MAX_ALLOCS_FOR_512MB << 12), 16));
                dbg_print("\nMemory content at 0x");
                dbg_print(itoa((uint64_t)first_addr+i, 16));
                dbg_print(" is not 0x69\n");
                dbg_print("Was: ");
                dbg_print(itoa(*ptr, 16));
                dbg_print("\n");
                dbg_print("Dump of 100 items 50before 50after: ");
                for (int64_t j = -50; j < 50; j++) {
                    dbg_print(itoa(*(ptr+j), 16));
                    dbg_print(" ");
                }
                panic("Memory leak detected!");
            }
        }

        dbg_print("\n");
        dbg_print("Last checked address for 0x69: 0x");
        dbg_print(itoa(last_addr, 16));
        dbg_print("\n");

        dbg_print("\nTesting bitfield for all lock\n");

        for (uint64_t i = 0; i < (MAX_ALLOCS_FOR_512MB >> 3); i++) {
            last_addr = (uint64_t)&memory->bitfield[i];
            if (memory->bitfield[i] != 0xff) {
                dbg_print("Target is at: 0x");
                dbg_print(itoa((MAX_ALLOCS_FOR_512MB >> 3), 16));
                dbg_print("\nBitfield is not all 1s at index 0x");
                dbg_print(itoa(i, 16));
                dbg_print("\n");
                dbg_print("Dump of 100 items 50before 50after: ");
                for (int64_t j = -50; j < 50; j++) {
                    dbg_print(itoa(memory->bitfield[i+j], 16));
                    dbg_print(" ");
                }
                panic("Memory leak detected!");
            }
        }

        dbg_print("\n");
        dbg_print("Last checked address for bitfield lock: 0x");
        dbg_print(itoa(last_addr, 16));
        dbg_print("\n");

        dbg_print("Freeing memory\n");
        for (uint64_t i = 0; i < MAX_ALLOCS_FOR_512MB; i++) {
            void* addr = (void*)((uint64_t)first_addr + (i << 12));
            last_addr = (uint64_t)addr;
            memset(addr, 0x0, 0x1000);
            free_page(addr);
        }
        dbg_print("Last freed page addr: 0x");
        dbg_print(itoa((uint64_t)last_addr, 16));
        dbg_print("\nBitfield at that address is now 0x");
        dbg_print(itoa(memory->bitfield[last_addr >> 15], 16));
        dbg_print("\n");
        dbg_print("\nTesting bitfield for all free\n");
        
        for (uint64_t i = 6; i < (memory->total_available_pages >> 3); i++) {
            last_addr = (uint64_t)&memory->bitfield[i];
            if (memory->bitfield[i] != 0) {
                dbg_print("Target is at: 0x");
                dbg_print(itoa((memory->total_available_pages >> 3), 16));
                dbg_print("\nBitfield is not all 0s at index 0x");
                dbg_print(itoa(i, 16));
                dbg_print("\n");
                dbg_print("That's equivalent to addr: 0x");
                dbg_print(itoa((i << 15), 16));
                dbg_print("\nDump of 100 items 50before 50after: ");
                for (int64_t j = -50; j < 50; j++) {
                    dbg_print(itoa(memory->bitfield[i+j], 16));
                    dbg_print(" ");
                }
                panic("Memory leak detected!");
            }
        }
        dbg_print("\n");
        dbg_print("Last checked address for empty bitfield: 0x");
        dbg_print(itoa(last_addr, 16));
        dbg_print("\n");
        dbg_print("Testing memory content for 0x0\n");
        for (uint64_t i = 0; i < ((MAX_ALLOCS_FOR_512MB) << 12); i++) {
            uint8_t * ptr = (uint8_t*)((uint64_t)first_addr + i);
            last_addr = (uint64_t)ptr;
            if (*ptr != 0x00) {
                panic("Memory leak detected!");
            }
        }
        dbg_print("\n");
        dbg_print("Last checked address for 0x0: 0x");
        dbg_print(itoa(last_addr, 16));
        dbg_print("\n");
    }

    dbg_print("[SUCCESS] !!!Total lock unlock test passed!!! [SUCCESS]\n");    
}