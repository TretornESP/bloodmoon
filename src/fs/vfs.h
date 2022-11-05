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

struct super_operations {
    void *nonempty;
        /*struct inode *(*alloc_inode) (struct super_block *sb);
        void (*destroy_inode) (struct inode *);
        void (*read_inode) (struct inode *);
        void (*dirty_inode) (struct inode *);
        void (*write_inode) (struct inode *, int);
        void (*put_inode) (struct inode *);
        void (*drop_inode) (struct inode *);
        void (*delete_inode) (struct inode *);
        void (*put_super) (struct super_block *);
        void (*write_super) (struct super_block *);
        int (*sync_fs) (struct super_block *, int);
        void (*write_super_lockfs) (struct super_block *);
        void (*unlockfs) (struct super_block *);
        int (*statfs) (struct super_block *, struct statfs *);
        int (*remount_fs) (struct super_block *, int *, char *);
        void (*clear_inode) (struct inode *);
        void (*umount_begin) (struct super_block *);
        int (*show_options) (struct seq_file *, struct vfsmount *);*/
};

struct super_block {
    char name[32];
    struct inode *root;
    uint64_t block_size;
    struct super_operations* operations;
    void * fs_extra;
};

struct file_system_type {
	char name[32];
    char (*register_partition)(const char*, uint32_t);
    uint8_t (*unregister_partition)(char);
    uint8_t (*detect)(const char *, uint32_t);
    struct file_system_type * next;
};

struct filesystem {
    struct super_block *sb;
    struct file_system_type *fs_type;
    struct filesystem *next;
};


struct inode_operations {
    void *nonempty;
	/*int (*create) (struct inode *,struct dentry *, umode_t, bool);
	struct dentry * (*lookup) (struct inode *,struct dentry *, unsigned int);
	int (*link) (struct dentry *,struct inode *,struct dentry *);
	int (*unlink) (struct inode *,struct dentry *);
	int (*symlink) (struct inode *,struct dentry *,const char *);
	int (*mkdir) (struct inode *,struct dentry *,umode_t);
	int (*rmdir) (struct inode *,struct dentry *);
	int (*mknod) (struct inode *,struct dentry *,umode_t,dev_t);
	int (*rename) (struct inode *, struct dentry *,
			struct inode *, struct dentry *, unsigned int);
	int (*readlink) (struct dentry *, char __user *,int);
	const char *(*get_link) (struct dentry *, struct inode *,
				 struct delayed_call *);
	int (*permission) (struct inode *, int);
	int (*get_acl)(struct inode *, int);
	int (*setattr) (struct dentry *, struct iattr *);
	int (*getattr) (const struct path *, struct kstat *, u32, unsigned int);
	ssize_t (*listxattr) (struct dentry *, char *, size_t);
	void (*update_time)(struct inode *, struct timespec *, int);
	int (*atomic_open)(struct inode *, struct dentry *, struct file *,
			unsigned open_flag, umode_t create_mode);
	int (*tmpfile) (struct inode *, struct dentry *, umode_t); */
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
    struct super_block *super_block;
    char  d_name[32];
} __attribute__((packed));

void register_filesystem(struct vfs_compatible *);
void init_vfs();

#endif