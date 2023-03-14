#ifndef _EXT2_PARTITION_H
#define _EXT2_PARTITION_H

#include "ext2.h"
#include <stdint.h>

struct ext2_partition {
    char name[32];
    char disk[32];
    uint32_t lba;
    uint32_t group_number;
    uint32_t sector_size;
    uint32_t bgdt_block;
    uint32_t sb_block;
    uint8_t flush_required;
    struct ext2_superblock_extended *sb;
    struct ext2_block_group_descriptor *gd;
    struct ext2_partition *next;
};

void ext2_disk_from_partition(char * destination, const char * partition);
uint8_t ext2_check_status(const char* disk);
struct ext2_partition * get_partition(struct ext2_partition* partition, const char * partno);
void ext2_dump_partition(struct ext2_partition* partition);
#endif /* _EXT2_PARTITION_H */