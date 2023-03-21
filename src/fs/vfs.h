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
    int (*register_partition)(const char*, uint32_t);
    uint8_t (*unregister_partition)(int);
    uint8_t (*detect)(const char *, uint32_t);
    int (*flush)(int);
    //TODO: Ampliar hasta cubrir generic/compat_vfs
    struct file_system_type * next;
};

struct inode {
    const char device_name[32];
    uint32_t inumber;
    uint8_t user;
    uint8_t group;
    uint64_t size;
    uint8_t mode;
    uint16_t opens;
    uint8_t permissions;
    uint64_t lba;
    uint64_t ctime;
    uint64_t mtime;
    uint64_t atime;  
};

struct inode_operations {
    void * nonempty;
/*
    int (*create) (struct inode *, struct dentry *, int, struct nameidata *);
    struct dentry * (*lookup) (struct inode *, struct dentry *, struct nameidata *);
    int (*link) (struct dentry *, struct inode *, struct dentry *);
    int (*unlink) (struct inode *, struct dentry *);
    int (*symlink) (struct inode *, struct dentry *, const char *);
    int (*mkdir) (struct inode *, struct dentry *, int);
    int (*rmdir) (struct inode *, struct dentry *);
    int (*mknod) (struct inode *, struct dentry *, int, dev_t);
    int (*rename) (struct inode *, struct dentry *,
                   struct inode *, struct dentry *);
    void *(*follow_link) (struct dentry *, struct nameidata *);
    void (*put_link) (struct dentry *, struct nameidata *, void *);
    int (*readlink) (struct dentry *, char __user *, int);
    void (*truncate) (struct inode *);
    int (*permission) (struct inode *, int, struct nameidata *);
    int (*setattr) (struct dentry *, struct iattr *);
    int (*getattr) (struct vfsmount *, struct dentry *, struct kstat *);
    int (*setxattr) (struct dentry *, const char *, const void *, size_t, int);
    ssize_t (*getxattr) (struct dentry *, const char *, void *, size_t);
    ssize_t (*listxattr) (struct dentry *, char *, size_t);
    int (*removexattr) (struct dentry *, const char *);
    void (*truncate_range) (struct inode *, loff_t, loff_t);
    int (*fiemap) (struct inode *, struct fiemap_extent_info *, u64, u64);
    int (*update_time) (struct inode *, struct timespec *, int);
    int (*atomic_open) (struct inode *, struct dentry *,
                        struct file *, unsigned open_flag,
                        umode_t create_mode, int *opened);
    int (*tmpfile) (struct inode *, struct dentry *, umode_t);
    int (*set_acl) (struct inode *, struct posix_acl *, int);
    int (*get_acl) (struct inode *, int);
    int (*fallocate) (struct inode *, int, loff_t, loff_t);
    int (*fadv
*/
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
    char name[48];
	uint32_t lba;
	uint32_t size;
	uint8_t status;
	uint8_t type;
    struct partition* next;
};

struct devmap {
    struct device * dev;
    struct partition * partitions;
    uint32_t partition_no;
};

struct mount {
    struct device* device;
    struct partition* partition;
    struct file_system_type* fst;
    int internal_index;
    struct mount * next;
};

struct dentry {
    uint16_t open_count;
    struct inode *inode;
    struct dentry* subdirs;
    struct dentry* parent;
    uint16_t subdirs_count;
    struct partition *partition;
    char  d_name[32];
};

void register_filesystem(struct vfs_compatible *);
void init_vfs();

#endif