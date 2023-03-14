#ifndef _GENERIC_EXT2_H
#define _GENERIC_EXT2_H
#include "../vfs_compat.h"
#include "ext2.h"

/*
const char * ext2_get_partition_name(struct ext2_partition * partition);
uint32_t ext2_count_partitions();
struct ext2_partition * ext2_get_partition_by_index(uint32_t index);

uint64_t ext2_get_file_size(struct ext2_partition* partition, const char* path);
uint8_t ext2_list_directory(struct ext2_partition* partition, const char * path);
uint8_t ext2_resize_file(struct ext2_partition* partition, uint32_t inode_index, uint32_t new_size);
uint32_t ext2_get_inode_index(struct ext2_partition* partition, const char* path);
uint8_t ext2_set_debug_base(const char* base);
uint16_t ext2_get_file_permissions(struct ext2_partition* partition, const char* path);

uint8_t ext2_debug(struct ext2_partition* partition);
uint8_t ext2_stacktrace();
uint8_t ext2_errors();


    char (*register_partition)(const char*, uint32_t);
    uint8_t (*unregister_partition)(char);
    uint8_t (*detect)(const char*, uint32_t);
    int (*flush)(void);

    int (*file_open)(const char*, int, int);
    int (*file_close)(int);
    int (*file_creat)(const char*, int);
    uint64_t (*file_read)(int, void*, uint64_t);
    uint64_t (*file_write)(int, void*, uint64_t);
    
    uint64_t (*file_seek)(int, uint64_t, int);
    int (*stat)(int, stat_t*);

    DIR (*dir_open)(const char*);
    int (*dir_close)(DIR);
    int (*dir_read)(DIR);
    DIR (*dir_creat)(const char*);

    int (*rename)(const char*, const char*);
    int (*remove)(const char*);
    int (*chmod)(const char*, int);
*/

#define MAX_OPEN_DIRS 256
#define MAX_REQUESTED_FILES 256

struct dir_s open_dirs[MAX_OPEN_DIRS];
struct info_s requested_files[MAX_REQUESTED_FILES];

struct partition_usage {
    uint8_t in_use;
    char name[32];
};

struct partition_usage partitions[26];

char* partition_name_from_char(char c) {
    if (c < 'a' || c > 'z') {
        return 0;
    }
    return partitions[c - 'a'].name;
}

char register_partition(const char* disk, uint32_t lba) {
    uint32_t i;
    for (i = 0; i < 26; i++) {
        if (partitions[i].in_use == 0) {
            partitions[i].in_use = 1;
            strcpy(partitions[i].name, disk);
            return 'a' + i;
        }
    }
    return 0;
}

uint8_t unregister_partition(char letter) {
    if (letter < 'a' || letter > 'z') {
        return 0;
    }
    partitions[letter - 'a'].in_use = 0;
    return 1;
}

uint8_t detect (const char* disk, uint32_t lba) {
    return ext2_search(disk, lba);
}

int file_open(const char* path, int flags, int mode) {
    return 0;
}

/*

int (*file_open)(const char*, int, int);
//Virtual function

int (*file_close)(int);
//Virtual function

int (*file_creat)(const char*, int);
uint8_t ext2_create_file(struct ext2_partition * partition, const char* path, uint32_t type, uint32_t permissions);

uint64_t (*file_read)(int, void*, uint64_t);
uint8_t ext2_read_file(struct ext2_partition * partition, const char * path, uint8_t * destination_buffer, uint64_t size, uint64_t skip);

uint64_t (*file_write)(int, void*, uint64_t);
uint8_t ext2_write_file(struct ext2_partition * partition, const char * path, uint8_t * source_buffer, uint64_t size, uint64_t skip);


uint64_t (*file_seek)(int, uint64_t, int);
//Virtual function


int (*file_sync)(int);
uint8_t ext2_sync(struct ext2_partition * partition);

DIR (*dir_open)(const char*);
//Virtual function

int (*dir_close)(DIR);
//Virtual function

int (*dir_read)(DIR);
//Virtual function

DIR (*dir_creat)(const char*);
//Virtual function

int (*rename)(const char*, const char*);
//Not implemented

int (*remove)(const char*);
uint8_t ext2_delete_file(struct ext2_partition* partition, const char * path);

int (*chmod)(const char*, int);
//Not implemented

int (*stat)(int, stat_t*);
//Virtual function

*/

struct vfs_compatible ext2_register = {
    .name = "EXT2",
    .register_partition = ext2_register_partition,
    .unregister_partition = ext2_unregister_partition,
    .detect = ext2_search,

    .dir_open = f32_compat_dir_open,
    .dir_close = f32_compat_dir_close,
    .dir_creat = f32_compat_dir_create,
    .dir_read = f32_compat_dir_read
};

struct vfs_compatible * ext2_registrar = &ext2_register;

#endif