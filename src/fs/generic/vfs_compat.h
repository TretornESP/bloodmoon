#ifndef _VFS_COMPAT_H
#define _VFS_COMPAT_H
#include <stdint.h>
#define VFS_COMPAT_FS_NAME_MAX_LEN 32

struct vfs_compatible {
    char name[VFS_COMPAT_FS_NAME_MAX_LEN];
    char (*register_partition)(const char*, uint32_t);
    uint8_t (*unregister_partition)(char);
    uint8_t (*detect)(const char*, uint32_t);

    int (*file_open)(const char*, int, int);
    int (*file_close)(int);
    int (*file_creat)(const char*, int);
    uint64_t (*file_read)(int, void*, uint64_t);
    uint64_t (*file_write)(int, void*, uint64_t);
    uint64_t (*file_seek)(int, uint64_t, int);
    int (*file_sync)(int);

    int (*dir_open)(const char*);
    int (*dir_close)(int);
    int (*dir_read)(int);
    int (*dir_creat)(const char*, int);

    int (*rename)(const char*, const char*);
    int (*remove)(const char*);
    int (*chmod)(const char*, int);

    //TODO: Link operations!!!

};

#endif