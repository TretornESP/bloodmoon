#ifndef _VFS_H
#define _VFS_H

#define FAT32_FS    0x1
#define EXT2_FS     0x2
#define NTFS_FS     0x3
#define UNKNOWN_FS  0x4

struct super_operations {
        struct inode *(*alloc_inode) (struct super_block *sb);
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
        int (*show_options) (struct seq_file *, struct vfsmount *);
};

struct inode_operations {
	int (*create) (struct inode *,struct dentry *, umode_t, bool);
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
	int (*tmpfile) (struct inode *, struct dentry *, umode_t);
};



struct boot_block {

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

struct super_block {
    const char name[32];
    struct inode *root;
    uint64_t block_size;
    struct super_operations* operations;
    uint8_t fs_type;
    void * fs_extra;
};

struct file_system_type {
	const char name[32];
    struct dentry *(*mount) (struct file_system_type *, int, const char *, void *);
    void (*kill_sb) (struct super_block *);
    struct file_system_type * next;
};

void alloc_super();
void init_vfs();

#endif