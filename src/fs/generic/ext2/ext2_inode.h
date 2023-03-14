#ifndef _EXT2_INODE_H
#define _EXT2_INODE_H

#include "ext2.h"
#include <stdint.h>

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

//Hack?
#define INODE_BLOCK_END          0xFFFFFFFF

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
    uint32_t i_sectors;             /* sector count */
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

void ext2_dump_all_inodes(struct ext2_partition* partition, const char* root_path);
void ext2_dump_inode(struct ext2_inode_descriptor_generic * inode);
void ext2_dump_inode_bitmap(struct ext2_partition * partition);
uint32_t ext2_allocate_inode(struct ext2_partition * partition);
uint32_t* ext2_load_block_list(struct ext2_partition* partition, uint32_t inode_number);
void ext2_debug_print_file_inode(struct ext2_partition* partition, uint32_t inode_number);
uint8_t ext2_delete_n_blocks(struct ext2_partition* partition, uint32_t inode_number, uint32_t blocks_to_remove);
struct ext2_inode_descriptor * ext2_initialize_inode(struct ext2_partition* partition, uint32_t inode_number, uint32_t type, uint32_t permissions);
uint8_t ext2_write_inode(struct ext2_partition* partition, uint32_t inode_number, struct ext2_inode_descriptor* inode);
struct ext2_inode_descriptor * ext2_read_inode(struct ext2_partition* partition, uint32_t inode_number);
void ext2_print_inode(struct ext2_inode_descriptor_generic* inode);
uint32_t ext2_inode_from_path_and_parent(struct ext2_partition* partition, uint32_t parent_inode, const char* path);
uint32_t ext2_path_to_inode(struct ext2_partition* partition, const char * path);
uint8_t ext2_delete_inode(struct ext2_partition* partition, uint32_t inode_number);
uint8_t ext2_delete_file_blocks(struct ext2_partition* partition, uint32_t inode_number);
#endif