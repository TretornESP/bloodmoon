#ifndef _VFS_COMPAT_H
#define _VFS_COMPAT_H
#include <stdint.h>
#define VFS_COMPAT_FS_NAME_MAX_LEN 32

struct vfs_compatible {
    char name[VFS_COMPAT_FS_NAME_MAX_LEN];
    char (*register_partition)(const char*, uint32_t);
    uint8_t (*unregister_partition)(char);
    uint8_t (*detect)(const char*, uint32_t);
};

#endif