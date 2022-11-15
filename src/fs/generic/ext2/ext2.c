#include "ext2.h"

#include "../../../util/dbgprinter.h"
#include "../../../util/printf.h"
#include "../../../drivers/disk/disk_interface.h"

char register_ext2_partition(const char* disk, uint32_t lba) {
    (void)disk;
    (void)lba;
    printf("register_ext2_partition Not implemented\n");
    return 0;
}

uint8_t unregstr_ext2_partition(char letter) {
    (void)letter;
    printf("unregstr_ext2_partition Not implemented\n");
    return 0;
}

uint8_t ext2_search(const char* name, uint32_t lba) {
    uint8_t bpb[1024];
    if (!disk_read(name, bpb, lba+2, 2)) return 0;

    struct ext2_superblock *sb = (struct ext2_superblock *)bpb;
    return (sb->s_magic == EXT2_SUPER_MAGIC);
}