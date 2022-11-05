//https://wiki.osdev.org/Ext2
#ifndef _EXT2_H
#define _EXT2_H
#include <stdint.h>


#define EXT2_SUPER_MAGIC    0xEF53
#define EXT2_NAME_LEN       255

//FS State
#define EXT2_FS_CLEAN       1
#define EXT2_FS_ERRORS      2

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

struct ext2_block_group_descriptor {
    uint32_t bg_block_bitmap;        /* Blocks bitmap block */
    uint32_t bg_inode_bitmap;       /* Inodes bitmap block */
    uint32_t bg_inode_table;        /* Inodes table block */
    uint16_t bg_free_blocks_count;  /* Free blocks count */
    uint16_t bg_free_inodes_count;  /* Free inodes count */
    uint16_t bg_used_dirs_count;    /* Directories count */
    uint8_t  bg_pad[14];            /* Padding to the end of the block */
} __attribute__((packed));

//Inode type and permissions
#define INODE_TYPE_FIFO          0x1000
#define INODE_TYPE_CHARDEV       0x2000
#define INODE_TYPE_DIR           0x4000
#define INODE_TYPE_BLOCKDEV      0x6000
#define INODE_TYPE_FILE          0x8000
#define INODE_TYPE_SYMLINK       0xA000
#define INODE_TYPE_SOCKET        0xC000

#define INODE_PERM_OTHER_EXEC    0x0001
#define INODE_PERM_OTHER_WRITE   0x0002
#define INODE_PERM_OTHER_READ    0x0004
#define INODE_PERM_GROUP_EXEC    0x0008
#define INODE_PERM_GROUP_WRITE   0x0010
#define INODE_PERM_GROUP_READ    0x0020
#define INODE_PERM_USER_EXEC     0x0040
#define INODE_PERM_USER_WRITE    0x0080
#define INODE_PERM_USER_READ     0x0100
#define INODE_PERM_STICKY        0x0200
#define INODE_PERM_SETGID        0x0400
#define INODE_PERM_SETUID        0x0800

//Inode flags
#define INODE_FLAG_SECRM         0x00000001
#define INODE_FLAG_KEEP_COPY     0x00000002
#define INODE_FLAG_FILE_COMPRESS 0x00000004
#define INODE_FLAG_SYNC          0x00000008
#define INODE_FLAG_IMMUTABLE     0x00000010
#define INODE_FLAG_APPEND        0x00000020
#define INODE_FLAG_NO_DUMP       0x00000040
#define INODE_FLAG_NO_ATIME      0x00000080
#define INODE_FLAG_HASH_INDEXED  0x00010000
#define INODE_FLAG_AFS_DIR       0x00020000
#define INODE_FLAG_JOURNAL_DATA  0x00040000

struct ext2_inode_descriptor_generic {
    uint16_t i_mode;                /* File mode */
    uint16_t i_uid;                 /* Low 16 bits of Owner Uid */
    uint32_t i_size;                /* Size in bytes */
    uint32_t i_atime;               /* Access time */
    uint32_t i_ctime;               /* Creation time */
    uint32_t i_mtime;               /* Modification time */
    uint32_t i_dtime;               /* Deletion Time */
    uint16_t i_gid;                 /* Low 16 bits of Group Id */
    uint16_t i_links_count;         /* Links count */
    uint32_t i_blocks;              /* Blocks count */
    uint32_t i_flags;               /* File flags */
    uint32_t i_osd1;                /* OS dependent 1 */
    uint32_t i_block[15];           /* Pointers to blocks */
    uint32_t i_generation;          /* File version (for NFS) */
    uint32_t i_file_acl;            /* File ACL */
    uint32_t i_dir_acl;             /* Directory ACL */
    uint32_t i_faddr;               /* Fragment address */
} __attribute__((packed));

struct ext2_inode_descriptor {
    struct ext2_inode_descriptor_generic id;
    uint8_t  i_osd2[12];            /* OS dependent 2 */
} __attribute__((packed));

struct ext2_inode_descriptor_linux {
    struct   ext2_inode_descriptor_generic id;
    uint8_t  i_fragment_no;          /* Fragment number */
    uint8_t  i_fragment_size;        /* Fragment size */
    uint16_t i_osd2;                /* OS dependent 2 */
    uint16_t i_high_uid;            /* High 16 bits of Owner Uid */
    uint16_t i_high_gid;            /* High 16 bits of Group Id */
    uint32_t i_reserved;
} __attribute__((packed));

struct ext2_inode_descriptor_hurd {
    struct   ext2_inode_descriptor_generic id;
    uint8_t  i_frag;               /* Fragment number */
    uint8_t  i_fsize;               /* Fragment size */
    uint16_t i_high_type_perm;      /* High 16 bits of Type and Permission */
    uint16_t i_high_uid;            /* High 16 bits of Owner Uid */
    uint16_t i_high_gid;            /* High 16 bits of Group Id */
    uint32_t i_author_id;           /* Author ID */
} __attribute__((packed));

struct ext2_inode_descriptor_masix {
    struct  ext2_inode_descriptor_generic id;
    uint8_t i_frag;                 /* Fragment number */
    uint8_t i_fsize;                /* Fragment size */
    uint8_t i_reserved[10];         /* Reserved */    
} __attribute__((packed));

struct ext2_directory_entry {
    uint32_t inode;                 /* Inode number */
    uint16_t rec_len;               /* Directory entry length */
    uint8_t  name_len;              /* Name length */
    uint8_t  file_type;             /* File type */
    char     name[EXT2_NAME_LEN];   /* File name */
} __attribute__((packed));

//Directory entry types
#define EXT2_DIR_TYPE_UNKNOWN   0
#define EXT2_DIR_TYPE_REGULAR   1
#define EXT2_DIR_TYPE_DIRECTORY 2
#define EXT2_DIR_TYPE_CHARDEV   3
#define EXT2_DIR_TYPE_BLOCKDEV  4
#define EXT2_DIR_TYPE_FIFO      5
#define EXT2_DIR_TYPE_SOCKET    6
#define EXT2_DIR_TYPE_SYMLINK   7

char 	register_ext2_partition(const char* disk, uint32_t lba);
uint8_t unregstr_ext2_partition(char letter);
uint8_t ext2_search(const char*, uint32_t);

#endif