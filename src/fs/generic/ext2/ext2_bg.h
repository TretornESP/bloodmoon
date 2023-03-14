#ifndef _EXT2_BG_H
#define _EXT2_BG_H

#include "ext2.h"
#include "ext2_sb.h"

#include <stdint.h>

struct ext2_block_group_descriptor {
    uint32_t bg_block_bitmap;        /* Blocks bitmap block */
    uint32_t bg_inode_bitmap;       /* Inodes bitmap block */
    uint32_t bg_inode_table;        /* Inodes table block */
    uint16_t bg_free_blocks_count;  /* Free blocks count */
    uint16_t bg_free_inodes_count;  /* Free inodes count */
    uint16_t bg_used_dirs_count;    /* Directories count */
    uint8_t  bg_pad[14];            /* Padding to the end of the block */
} __attribute__((packed));

int32_t ext2_operate_on_bg(struct ext2_partition * partition, uint8_t (*callback)(struct ext2_partition *, struct ext2_block_group_descriptor*, uint32_t));
uint8_t ext2_flush_bg(struct ext2_partition* partition, struct ext2_block_group_descriptor* bg, uint32_t bgid);
uint8_t ext2_dump_bg(struct ext2_partition* partition, struct ext2_block_group_descriptor * bg, uint32_t id);
uint8_t ext2_bg_has_free_inodes(struct ext2_partition * partition, struct ext2_block_group_descriptor * bg, uint32_t id);
void ext2_dump_all_bgs(struct ext2_partition* partition);
#endif /* _EXT2_BG_H */