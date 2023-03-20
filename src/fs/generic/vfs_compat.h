#ifndef _VFS_COMPAT_H
#define _VFS_COMPAT_H
#include <stdint.h>
#define VFS_COMPAT_FS_NAME_MAX_LEN 32

#define O_RDONLY 0x0000
#define O_WRONLY 0x0001
#define O_RDWR 0x0002

#define O_APPEND 0x0008
#define O_CREAT 0x0200
#define O_TRUNC 0x0400
#define O_EXCL 0x0800

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define S_IFMT 0170000
#define S_IFSOCK 0140000
#define S_IFLNK 0120000
#define S_IFREG 0100000
#define S_IFBLK 0060000
#define S_IFDIR 0040000
#define S_IFCHR 0020000
#define S_IFIFO 0010000
#define S_ISUID 0004000
#define S_ISGID 0002000
#define S_ISVTX 0001000
#define S_IRWXU 0000700
#define S_IRUSR 0000400
#define S_IWUSR 0000200
#define S_IXUSR 0000100
#define S_IRWXG 0000070
#define S_IRGRP 0000040
#define S_IWGRP 0000020
#define S_IXGRP 0000010
#define S_IRWXO 0000007
#define S_IROTH 0000004
#define S_IWOTH 0000002
#define S_IXOTH 0000001

#define S_ISLNK(m) (((m)&S_IFMT) == S_IFLNK)
#define S_ISREG(m) (((m)&S_IFMT) == S_IFREG)
#define S_ISDIR(m) (((m)&S_IFMT) == S_IFDIR)
#define S_ISCHR(m) (((m)&S_IFMT) == S_IFCHR)
#define S_ISBLK(m) (((m)&S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m)&S_IFMT) == S_IFIFO)
#define S_ISSOCK(m) (((m)&S_IFMT) == S_IFSOCK)

struct stat {
    uint64_t st_dev;
    uint64_t st_ino;
    uint64_t st_mode;
    uint64_t st_nlink;
    uint64_t st_uid;
    uint64_t st_gid;
    uint64_t st_rdev;
    uint64_t st_size;
    uint64_t st_blksize;
    uint64_t st_blocks;
    uint64_t st_atime;
    uint64_t st_mtime;
    uint64_t st_ctime;
};
typedef struct stat stat_t;

struct dir {
    int fd;
    uint32_t entries;
    uint32_t index;
};

typedef struct dir dir_t;

struct vfs_compatible {
    char name[VFS_COMPAT_FS_NAME_MAX_LEN];
    int (*register_partition)(const char*, uint32_t);
    uint8_t (*unregister_partition)(int);
    uint8_t (*detect)(const char*, uint32_t);
    int (*flush)(int);

    int (*file_open)(const char*, int, int);
    int (*file_close)(int);
    int (*file_creat)(const char*, int);
    uint64_t (*file_read)(int, void*, uint64_t);
    uint64_t (*file_write)(int, void*, uint64_t);
    uint64_t (*file_seek)(int, uint64_t, int);
    int (*stat)(int, stat_t*);

    dir_t (*dir_open)(const char*);
    int (*dir_close)(dir_t);
    int (*dir_read)(dir_t);
    dir_t (*dir_creat)(const char*);

    int (*rename)(const char*, const char*);
    int (*remove)(const char*);
    int (*chmod)(const char*, int);

    //TODO: Link operations!!!

};

#endif