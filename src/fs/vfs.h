#ifndef _VFS_H
#define _VFS_H

#include <stdint.h>

#define FAT32_FS    0x1
#define EXT2_FS     0x2
#define NTFS_FS     0x3
#define UNKNOWN_FS  0x4

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
    struct dentry *(*mount) (struct file_system_type *, int, const char *, void *);
    void (*kill_sb) (struct super_block *);
    struct file_system_type * next;
    uint8_t type;
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

void init_vfs();

#endif