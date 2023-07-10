#include "fadt.h"
#include "../../util/printf.h"

void dump_fadt(struct fadt_header* fadt) {
    printf("FADT:\n");
    return;
}

void register_fadt(struct fadt_header* fadt, char* (*cb)(void*, uint8_t, uint64_t)) {
    (void)cb;
    dump_fadt(fadt);
    return;
}