#include "tests.h"
#include "../memory/memory.h"
#include "../memory/paging.h"
#include "../util/string.h"
#include "../util/dbgprinter.h"
#include "../util/printf.h"

#define TEST_MAGIC 0xdeadbeef
#define TEST_ADDR1 0xffffffffdeadb000
#define TEST_ADDR2 0xffffffffdeadd000

void pagetest() {
    init_memory();
    init_paging();

    uint64_t* ptr1 = (uint64_t*)TEST_ADDR1;
    uint64_t* ptr2 = (uint64_t*)TEST_ADDR2;

    void * target = request_page();
    memset(target, 0x0, 0x1000);

    map_memory((void*)TEST_ADDR1, target);
    map_memory((void*)TEST_ADDR2, target);

    *ptr1 = TEST_MAGIC;

    if (*ptr1 != TEST_MAGIC) {
        printf("[TEST FAILED] pagetest: ptr1 != TEST_MAGIC [TEST FAILED]\n");
    }
    if (*ptr1 != *ptr2) {
        panic("[TEST FAILED] pagetest: ptr1 != ptr2 [TEST FAILED]\n");
    }
    printf("[SUCCESS] !!!Paging test passed!!! [SUCCESS]\n");    
}