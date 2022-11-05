#ifndef _GENERIC_F32_H
#define _GENERIC_F32_H
#include "../vfs_compat.h"
#include "fat32.h"

struct vfs_compatible fat32_register = {
    .name = "FAT32",
    .register_partition = register_fat32_partition,
    .unregister_partition = unregstr_fat32_partition,
    .detect = fat_search
};

struct vfs_compatible * fat32_registrar = &fat32_register;

#endif