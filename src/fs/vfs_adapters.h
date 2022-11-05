#ifndef _VFS_ADAPTERS_H
#define _VFS_ADAPTERS_H
#include <stdint.h>
#include "vfs.h"

//Fat32 adapters
struct dentry* vfs_f32_mount     (struct file_system_type *, int, const char *, void *);
void           vfs_f32_kill_sb   (struct super_block *);

#endif