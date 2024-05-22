#include "tests.h"
#include "../io/interrupts.h"
#include "../memory/heap.h"
#include "../util/printf.h"
#include "../util/rand.h"
#include "../util/string.h"
#include "../util/dbgprinter.h"
#include "../memory/memory.h"
#include "../memory/paging.h"
#include "../arch/simd.h"

#define TEST_ROUNDS 250
#define TEST_MAX_SIZE 0x112340
#define TEST_VALUE 0x41

extern void enable_avx(void);
extern void enable_sse(void);

void heaptest() {
    init_simd();
    init_memory();
    init_paging();
    init_heap();
    init_interrupts(1);
    
    printf("Heap test started\n");
    void * allocations[TEST_ROUNDS];
    for (int i = 0; i < TEST_ROUNDS; i++) {
        if (i%1000 == 0) {
            printf("Round %d\n", i);
        }
        int size = rand() % TEST_MAX_SIZE;
        allocations[i] = kmalloc(size + 0x1000);
        memset(allocations[i], TEST_VALUE, size);
    }
    printf("Random free\n");
    int freeCount = 0;
    for (int i = 0; i < TEST_ROUNDS; i++) {
        if (rand() & 1) {kfree(allocations[i]); freeCount++;}
    }
    printf("Free count: %d\n", freeCount);
    printf("Random reallocation\n");
    for (int i = 0; i < TEST_ROUNDS; i++) {
        if (i%1000 == 0) {
            printf("Round %d\n", i);
        }
        int size = rand() % TEST_MAX_SIZE;
        void * ptr = kmalloc(size + 0x1000);
        memset(ptr, TEST_VALUE, size);
    }
    printf("Heap test finished\n");
    while(1);
}