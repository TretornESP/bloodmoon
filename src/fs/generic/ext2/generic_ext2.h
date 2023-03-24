#ifndef _GENERIC_EXT2_H
#define _GENERIC_EXT2_H
#include "../vfs_compat.h"
#include "ext2.h"
#include "../../../util/printf.h"
#include "../../../util/dbgprinter.h"

#define MAX_EXT2_PARTITIONS 32
#define MAX_EXT2_OPEN_FILES 65536

struct ext2_partition* ext2_partitions[MAX_EXT2_PARTITIONS] = {0};
//-1 cannot return the object
// 0 all okey
// 1 error

int ext2_compat_register_partition(const char* drive, uint32_t lba) {
    for (int i = 0; i < MAX_EXT2_PARTITIONS; i++) {
        if (ext2_partitions[i] == 0) {
            ext2_partitions[i] = ext2_register_partition(drive, lba);
            if (ext2_partitions[i] == 0) {
                panic("ext2: failed to register partition\n");
                ext2_stacktrace();
                while(1);
            }
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

void ext2_compat_debug() {
    ext2_stacktrace();
}

int ext2_compat_file_open(int partno, const char* path, int flags, int mode) {
    if (partno < 0 || partno >= MAX_EXT2_PARTITIONS) 
        return -1;
    struct ext2_partition * partition = ext2_partitions[partno];
    if (partition == 0)
        return -1;

    return get_fd(path, flags, mode);
    return -1;
}

int ext2_compat_file_close(int partno, int fd) {
    if (partno < 0 || partno >= MAX_EXT2_PARTITIONS) 
        return -1;
    struct ext2_partition * partition = ext2_partitions[partno];
    if (partition == 0)
        return -1;

    return release_fd(fd);
}

int ext2_compat_file_creat(int partno, const char* path, int mode) {return -1;}

uint64_t ext2_compat_file_read(int partno, int fd, void* buffer, uint64_t size) {
    if (partno < 0 || partno >= MAX_EXT2_PARTITIONS) 
        return -1;
    struct ext2_partition * partition = ext2_partitions[partno];
    if (partition == 0)
        return -1;

    struct file_descriptor_entry * entry = vfs_compat_get_file_descriptor(fd);
    return ext2_read_file(partition, entry->name, buffer, size, entry->offset);
}

uint64_t ext2_compat_file_write(int partno, int fd, void* buffer, uint64_t size) {return 1;}
uint64_t ext2_compat_file_seek(int partno, int fd, uint64_t offset, int whence) {return 1;}
int ext2_compat_stat(int partno, int fd, stat_t* st) {return -1;}

dir_t ext2_compat_dir_open(int partno, const char* path) {
    
    dir_t dir = {.fd = -1, .entries = 0, .index = 0};
    if (partno < 0 || partno >= MAX_EXT2_PARTITIONS) 
        goto end;
    struct ext2_partition * partition = ext2_partitions[partno];
    if (partition == 0)
        goto end;

    ext2_list_directory(partition, path);
end:
    return dir;
}

int ext2_compat_dir_close(int partno, dir_t dir) {return -1;}
int ext2_compat_dir_read(int partno, dir_t dir) {return -1;}
dir_t ext2_compat_dir_creat(int partno, const char* path) {struct dir d; d.fd = -1; return d;}
int ext2_compat_rename(int partno, const char* path, const char* newpath) {return -1;}
int ext2_compat_remove(int partno, const char* path) {return -1;}
int ext2_compat_chmod(int partno, const char* path, int mode) {return -1;}

struct vfs_compatible ext2_register = {
    .name = "EXT2",
    .register_partition = ext2_compat_register_partition,
    .unregister_partition = ext2_compat_unregister_partition,
    .detect = ext2_compat_detect,
    .flush = ext2_compat_flush,
    .debug = ext2_compat_debug,
    .file_open = ext2_compat_file_open,
    .file_close = ext2_compat_file_close,
    .file_creat = ext2_compat_file_creat,
    .file_read = ext2_compat_file_read,
    .file_write = ext2_compat_file_write,
    .file_seek = ext2_compat_file_seek,
    .file_stat = ext2_compat_stat,
    .rename = ext2_compat_rename,
    .remove = ext2_compat_remove,
    .chmod = ext2_compat_chmod,
    .dir_open = ext2_compat_dir_open,
    .dir_close = ext2_compat_dir_close,
    .dir_creat = ext2_compat_dir_creat,
    .dir_read = ext2_compat_dir_read
};

struct vfs_compatible * ext2_registrar = &ext2_register;

#endif