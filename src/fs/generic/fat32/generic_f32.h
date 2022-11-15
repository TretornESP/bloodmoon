#ifndef _GENERIC_F32_H
#define _GENERIC_F32_H
#include "../vfs_compat.h"
#include "fat32.h"
#define MAX_OPEN_DIRS 256
#define MAX_REQUESTED_FILES 256

struct dir_s open_dirs[MAX_OPEN_DIRS];
struct info_s requested_files[MAX_REQUESTED_FILES];
int last_open_file = 0;
int last_open_dir = 0;

int f32_compat_dir_open(const char* dname) {
  
    if (last_open_dir >= MAX_OPEN_DIRS) 
        return -1;
    
    fstatus result = fat_dir_open(&open_dirs[last_open_dir], dname, 0);

    if (result != FSTATUS_OK)
        return -1;

    return last_open_dir++;
}

int f32_compat_dir_close(int dindex) {
    if (fat_dir_close(&open_dirs[dindex]) != FSTATUS_OK)
        return -1;
    
    return 0;
}

int f32_compat_dir_read(int dindex) {

}

int f32_compat_dir_create(const char* dname, int mode) {
    
}

struct vfs_compatible fat32_register = {
    .name = "FAT32",

    .register_partition = register_fat32_partition,
    .unregister_partition = unregstr_fat32_partition,
    .detect = fat_search,

    .dir_open = f32_compat_dir_open,
    .dir_close = f32_compat_dir_close,
    .dir_creat = f32_compat_dir_create,
    .dir_read = f32_compat_dir_read
};

struct vfs_compatible * fat32_registrar = &fat32_register;

#endif