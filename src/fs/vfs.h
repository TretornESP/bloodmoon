#ifndef _VFS_H
#define _VFS_H

#include <stdint.h>
#include "generic/vfs_compat.h"
#include "../dev/devices.h"

/// MBR and boot sector
#define MBR_BOOTSTRAP		0
#define MBR_BOOTSTRAP_SIZE	446
#define MBR_PARTITION		446
#define MBR_PARTITION_SIZE	16
#define MBR_BOOT_SIG		510
#define MBR_BOOT_SIG_VALUE	0xAA55

#define PAR_STATUS			0
#define PAR_TYPE			4
#define PAR_LBA				8
#define PAR_SIZE			12

struct file_system_type {
	char name[32];
    char (*register_partition)(const char*, uint32_t);
    uint8_t (*unregister_partition)(char);
    uint8_t (*detect)(const char *, uint32_t);
    struct file_system_type * next;
};

struct inode {
    const char device_name[32];
    uint8_t user;
    uint8_t group;
    uint64_t size;
    uint8_t permissions;
    uint64_t lba;
    uint64_t ctime;
    uint64_t mtime;
    uint64_t atime;  
};

struct dentry_operations {
        void *nonempty;
/*
        int (*d_revalidate) (struct dentry *, int);
        int (*d_hash) (struct dentry *, struct qstr *);
        int (*d_compare) (struct dentry *, struct qstr *, struct qstr *);
        int (*d_delete) (struct dentry *);
        void (*d_release) (struct dentry *);
        void (*d_iput) (struct dentry *, struct inode *);*/
};

struct partition {
	uint32_t lba;
	uint32_t size;
	uint8_t status;
	uint8_t type;
    struct partition* next;
};

struct devmap {
    struct device * dev;
    struct partition * partitions;
};

struct dentry {
    uint16_t open_count;
    struct inode *inode;
    struct dentry* subdirs;
    struct dentry* parent;
    uint16_t subdirs_count;
    struct dentry_operations *operations;
    struct partition *partition;
    char  d_name[32];
};

void register_filesystem(struct vfs_compatible *);
void init_vfs();

#endif