#ifndef _GENERIC_EXT2_H
#define _GENERIC_EXT2_H
#include "../vfs_compat.h"
#include "ext2.h"

#define MAX_EXT2_PARTITIONS 32

struct ext2_partition* ext2_partitions[MAX_EXT2_PARTITIONS] = {0};

//-1 cannot return the object
// 0 all okey
// 1 error

int ext2_compat_register_partition(const char* drive, uint32_t lba) {
    for (int i = 0; i < MAX_EXT2_PARTITIONS; i++) {
        if (ext2_partitions[i] == 0) {
            ext2_partitions[i] = ext2_register_partition(drive, lba);
            return i;
        }
    }
    return -1;
}

uint8_t ext2_compat_unregister_partition(int index) {
    if (ext2_partitions[index] != 0) {
        ext2_unregister_partition(ext2_partitions[index]);
        ext2_partitions[index] = 0;
        return 0;
    }
    return 1;
}

uint8_t ext2_compat_detect(const char* disk , uint32_t lba) {
    return ext2_search(disk, lba);
}

int ext2_compat_flush(int index) {
    if (index < 0 || index >= MAX_EXT2_PARTITIONS)
        return -1;

    struct ext2_partition* partition = ext2_partitions[index];
    return (int)ext2_sync(partition);
}

int ext2_compat_file_open(const char* path, int flags, int mode) {return -1;}
int ext2_compat_file_close(int fd) {return -1;}
int ext2_compat_file_creat(const char* path, int mode) {return -1;}
uint64_t ext2_compat_file_read(int fd, void* buffer, uint64_t size) {return 1;}
uint64_t ext2_compat_file_write(int fd, void* buffer, uint64_t size) {return 1;}
uint64_t ext2_compat_file_seek(int fd, uint64_t offset, int whence) {return 1;}
int ext2_compat_stat(int fd, stat_t* st) {return -1;}
dir_t ext2_compat_dir_open(const char* path) {struct dir d; d.fd = -1; return d;}
int ext2_compat_dir_close(dir_t dir) {return -1;}
int ext2_compat_dir_read(dir_t dir) {return -1;}
dir_t ext2_compat_dir_creat(const char* path) {struct dir d; d.fd = -1; return d;}
int ext2_compat_rename(const char* path, const char* newpath) {return -1;}
int ext2_compat_remove(const char* path) {return -1;}
int ext2_compat_chmod(const char* path, int mode) {return -1;}

struct vfs_compatible ext2_register = {
    .name = "EXT2",
    .register_partition = ext2_compat_register_partition,
    .unregister_partition = ext2_compat_unregister_partition,
    .detect = ext2_compat_detect,
    .flush = ext2_compat_flush
    //.dir_open = ext2_compat_dir_open,
    //.dir_close = ext2_compat_dir_close,
    //.dir_creat = ext2_compat_dir_create,
    //.dir_read = ext2_compat_dir_read
};

struct vfs_compatible * ext2_registrar = &ext2_register;

#endif