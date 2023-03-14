#ifndef _EXT2_SB_H
#define _EXT2_SB_H

#include "ext2.h"
#include "ext2_partition.h"
#include <stdint.h>

//FS State
#define EXT2_FS_CLEAN       1
#define EXT2_FS_ERRORS      2

#define EXT2_DIRECT_BLOCKS 12
#define EXT2_INDIRECT_BLOCKS(blocksize, sizeo_of_entry) ((blocksize / sizeo_of_entry))
#define EXT2_DOUBLE_INDIRECT_BLOCKS(blocksize, sizeo_of_entry) ((blocksize / sizeo_of_entry) * (blocksize / sizeo_of_entry))
#define EXT2_TRIPLE_INDIRECT_BLOCKS(blocksize, sizeo_of_entry) ((blocksize / sizeo_of_entry) * (blocksize / sizeo_of_entry) * (blocksize / sizeo_of_entry))

//Error handling methods
#define EXT2_IGNORE_ERRORS  1
#define EXT2_REMOUNT_RO     2
#define EXT2_KERNEL_PANIC   3

//Creator OS id
#define EXT2_LINUX          0
#define EXT2_HURD           1
#define EXT2_MASIX          2
#define EXT2_FREEBSD        3
#define EXT2_LITES          4

//Optional feature flags
#define PREALLOCATE_BLOCKS_FLAG     0x0001
#define AFS_SERVER_INODES_FLAG      0x0002
#define HAS_JOURNAL_FLAG            0x0004
#define INODES_EXT_ATTRIBS_FLAG     0x0008
#define CAN_RESIZE_FS_FLAG          0x0010
#define HASH_DIRECTORY_INDEX_FLAG   0x0020

//Required feature flags
#define COMPRESSION_USED            0x0001
#define DIRECTORY_ENTRIES_TYPE_FILE 0x0002
#define FS_NEEDS_JOURNAL_REPLAY     0x0004
#define FS_USES_JOURNAL_DEVICE      0x0008

//Read-only feature flags
#define SPARSE_SUPERBLOCKS          0x0001
#define FS_USES_64BIT_FILESIZE      0x0002
#define DIR_ENTRIES_TYPE_BTREE      0x0004

struct ext2_superblock {
    uint32_t s_inodes_count;        /* Inodes count */
    uint32_t s_blocks_count;        /* Blocks count */
    uint32_t s_r_blocks_count;      /* Reserved blocks count */
    uint32_t s_free_blocks_count;   /* Free blocks count */
    uint32_t s_free_inodes_count;   /* Free inodes count */
    uint32_t s_first_sb_block;      /* First superblock Block */
    uint32_t s_log_block_size;      /* Block size */
    uint32_t s_log_frag_size;       /* Fragment size */
    uint32_t s_blocks_per_group;    /* # Blocks per group */
    uint32_t s_frags_per_group;     /* # Fragments per group */
    uint32_t s_inodes_per_group;    /* # Inodes per group */
    uint32_t s_mtime;               /* Mount time */
    uint32_t s_wtime;               /* Write time */
    uint16_t s_mnt_count;           /* Mount count */
    uint16_t s_max_mnt_count;       /* Maximal mount count */
    uint16_t s_magic;               /* Magic signature */
    uint16_t s_state;               /* File system state */
    uint16_t s_errors;              /* Behaviour when detecting errors */
    uint16_t s_minor_rev_level;     /* minor revision level */
    uint32_t s_lastcheck;           /* time of last check */
    uint32_t s_checkinterval;       /* max. time between checks */
    uint32_t s_creator_os;          /* OS */
    uint32_t s_rev_level;           /* Revision level */
    uint16_t s_def_resuid;          /* Default uid for reserved blocks */
    uint16_t s_def_resgid;          /* Default gid for reserved blocks */
} __attribute__((packed));

struct ext2_superblock_extended {
    struct ext2_superblock sb;
    uint32_t s_first_ino;           /* First non-reserved inode */
    uint16_t s_inode_size;          /* size of inode structure */
    uint16_t s_block_group_nr;      /* block group # of this superblock */
    uint32_t s_feature_compat;      /* compatible feature set */
    uint32_t s_feature_incompat;    /* incompatible feature set */
    uint32_t s_feature_ro_compat;   /* readonly-compatible feature set */
    uint8_t  s_uuid[16];            /* 128-bit uuid for volume */
    char     s_volume_name[16];     /* volume name */
    char     s_last_mounted[64];    /* directory where last mounted */
    uint32_t s_algo_bitmap;         /* For compression */
    uint8_t  s_prealloc_blocks;     /* Nr of blocks to try to preallocate*/
    uint8_t  s_prealloc_dir_blocks; /* Nr to preallocate for dirs */
    uint16_t s_padding1;
    uint8_t  s_journal_uuid[16];    /* uuid of journal superblock */
    uint32_t s_journal_inum;        /* inode number of journal file */
    uint32_t s_journal_dev;         /* device number of journal file */
    uint32_t s_last_orphan;         /* start of list of inodes to delete */
    uint8_t  s_unused[788];         /* Padding to the end of the block */
} __attribute__((packed));

uint8_t ext2_flush_sb(struct ext2_partition* partition, struct ext2_block_group_descriptor* bg, uint32_t bgid);
void ext2_dump_sb(struct ext2_partition* partition);
#endif /* _EXT2_SB_H */