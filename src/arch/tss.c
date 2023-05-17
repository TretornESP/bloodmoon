#include "tss.h"
#include "../util/string.h"
#include "../memory/heap.h"

static struct tss tss[CPU_MAX];

void tss_init() {
    memset(tss, 0, sizeof(struct tss) * CPU_MAX);
}

struct tss * get_tss(uint8_t cpu) {
    return &tss[cpu];
}

struct tss * get_default_tss() {
    return get_tss(TSS_DEFAULT_CPU);
}

void prepare_tss(struct tss* tss) {
    memset(tss, 0, sizeof(struct tss));
    for (int i = 0; i < TSS_IST_SIZE; i++) {
        tss->ist[i] = (uint64_t)calloc(1, TSS_IST_STACK_SIZE);
    }

    tss->iopb = sizeof(struct tss); // No IO ports
}