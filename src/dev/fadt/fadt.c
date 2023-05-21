#include "fadt.h"
#include "printf.h"

void dump_fadt(struct fadt_header* fadt) {
    (void)fadt;
    return;
}

void register_fadt(struct fadt_header* fadt, char* (*cb)(void*, uint8_t, uint64_t)) {
    (void)cb;
    dump_fadt(fadt);
    return;
}