#include "ext2.h"

#include "../../../util/dbgprinter.h"
#include "../../../util/printf.h"
#include "../../../memory/heap.h"
#include "../../../util/string.h"
#include "../../../drivers/disk/disk_interface.h"
#define DIVIDE_ROUNDED_UP(x, y) (1 + (x-1) / y)
struct ext2_partition * ext2_partition_head = 0x0;

struct ext2_partition * init_ext2_partition_header() {
    struct ext2_partition * part = malloc(sizeof(struct ext2_partition));
    memset((void*)part, 0, sizeof(struct ext2_partition));
    return part;
}

char register_ext2_partition(const char* disk, uint32_t lba) {
    printf("[EXT2] Registering partition %s:%d\n", disk, lba);
    uint8_t superblock_buffer[1024];
    if (!disk_read(disk, superblock_buffer, lba+2, 2)) {
        printf("[EXT2] Failed to read superblock\n");
        return 0;
    }
    struct ext2_superblock * superblock = (struct ext2_superblock*)superblock_buffer;
    
    uint32_t block_groups_first  = DIVIDE_ROUNDED_UP(superblock->s_blocks_count, superblock->s_blocks_per_group);
    uint32_t block_groups_second = DIVIDE_ROUNDED_UP(superblock->s_inodes_count, superblock->s_inodes_per_group);

    if (block_groups_first != block_groups_second) {
        printf("ext2: asdasdlock_groups_first != block_groups_second\n");
        printf("ext2: block_groups_first = %d, block_groups_second = %d\n", block_groups_first, block_groups_second);
        return 0;
    }

    uint32_t group_descriptor_size = block_groups_first * sizeof(struct ext2_block_group_descriptor);
    uint32_t group_descriptor_sectors = DIVIDE_ROUNDED_UP(group_descriptor_size, 512);

    struct ext2_block_group_descriptor * block_group_descriptor = malloc(group_descriptor_size);
    if (!disk_read(disk, (uint8_t*)block_group_descriptor, lba+4, group_descriptor_sectors)) return 0;
    
    struct ext2_partition * partition = ext2_partition_head;
    if (partition == 0) {
        ext2_partition_head = init_ext2_partition_header();
        partition = ext2_partition_head;
    } else {
        while (partition->next != 0) {
            partition = partition->next;
        }
        partition->next = init_ext2_partition_header();
        partition = partition->next;
    }

    partition->group_number = block_groups_first;
    partition->sb = malloc(1024);
    memcpy(partition->sb, superblock, 1024);
    partition->gd = malloc(group_descriptor_size);
    memcpy(partition->gd, block_group_descriptor, group_descriptor_size);

    printf("[EXT2] Partition %s has: %d groups\n", disk, block_groups_first);
    return 0;
}

uint8_t unregstr_ext2_partition(char letter) {
    (void)letter;
    printf("[EXT2] unregstr_ext2_partition Not implemented\n");
    return 0;
}

uint8_t ext2_search(const char* name, uint32_t lba) {
    uint8_t bpb[1024];
    if (!disk_read(name, bpb, lba+2, 2)) return 0;

    struct ext2_superblock *sb = (struct ext2_superblock *)bpb;
    return (sb->s_magic == EXT2_SUPER_MAGIC);
}