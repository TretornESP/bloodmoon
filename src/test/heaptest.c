#include "tests.h"
#include "../memory/heap.h"
#include "../util/printf.h"
#include "../memory/memory.h"
#include "../memory/paging.h"

void heaptest() {
    
    init_memory();
    init_paging();
    init_heap();

    printf("Malloced address: 0x%llx\n", (uint64_t)malloc(0x8000));
    void * address = malloc(0x8000);
    printf("Malloced address: 0x%llx\n", (uint64_t)address);
    printf("Malloced address: 0x%llx\n", (uint64_t)malloc(0x100));
    free(address);
    printf("Freed address: 0x%llx\n", (uint64_t)address);

    void * address2 = malloc(0x100);

    printf("Malloced address: 0x%llx\n", (uint64_t)address2);

    if (address2 == address) {
        printf("[SUCCESS] !!!Heap tests passed!!! [SUCCESS]\n");
    } else {
        printf("[TEST FAILED] address2 != address [TEST FAILED]\n");
    }

}