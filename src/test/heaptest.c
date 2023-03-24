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

#define TEST_ROUNDS 45000
#define TEST_MAX_SIZE 0x1234
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
    set_debug_msg("Initial allocation");
    for (int i = 0; i < TEST_ROUNDS; i++) {
        int size = rand() % TEST_MAX_SIZE;
        allocations[i] = malloc(size);
        memset(allocations[i], TEST_VALUE, size);
    }
    set_debug_msg("Random free");
    for (int i = 0; i < TEST_ROUNDS; i++) {
        if (rand() & 1) free(allocations[i]);
    }
    set_debug_msg("Random reallocation");
    char msg_buffer[128] = {0};
    memset(msg_buffer, 0, 128);
    for (int i = 0; i < TEST_ROUNDS; i++) {
        //memset(msg_buffer, 0, 128);
        //sprintf(msg_buffer, "Round %d", i);
        //set_debug_msg(msg_buffer);
        int size = rand() % TEST_MAX_SIZE;
        void * ptr = malloc(size);
        memset(ptr, TEST_VALUE, size);
    }

}