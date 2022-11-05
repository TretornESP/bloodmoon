#ifndef _GENERIC_EXT2_H
#define _GENERIC_EXT2_H
#include "../vfs_compat.h"
#include "ext2.h"

struct vfs_compatible ext2_register = {
    .name = "EXT2",
    .register_partition = register_ext2_partition,
    .unregister_partition = unregstr_ext2_partition,
    .detect = ext2_search
};

struct vfs_compatible * ext2_registrar = &ext2_register;

#endif