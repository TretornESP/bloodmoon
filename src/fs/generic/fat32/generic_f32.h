#ifndef _GENERIC_F32_H
#define _GENERIC_F32_H
#include "../vfs_compat.h"
#include "fat32.h"
#define MAX_OPEN_DIRS 256
#define MAX_FILES_PER_DIR 4096
#define MAX_FAT32_PARTITIONS 32
struct f32_dir_usage {
    int in_use;
    struct dir_s dir;
};
struct f32_dir_usage f32_compat_open_dirs[MAX_OPEN_DIRS];

char f32_compat_partitions[MAX_FAT32_PARTITIONS] = {0};

dir_t f32_compat_dir_open(const char* dname) {
  
    dir_t directory = {
        .fd = -1,
        .index = 0,
        .entries = 0
    };
    
    int index = 0;
    for (index = 0; index < MAX_OPEN_DIRS; index++) {
        if (f32_compat_open_dirs[index].in_use == 0) {
            f32_compat_open_dirs[index].in_use = 1;
            break;
        }
    }

    fstatus result = fat_dir_open(&(f32_compat_open_dirs[index].dir), dname, 0);

    if (result != FSTATUS_OK) {
        return directory;
    }

	struct dir_s dir = f32_compat_open_dirs[index].dir;
	struct info_s* info = (struct info_s *)malloc(sizeof(struct info_s));
	memset(info, 0, sizeof(struct info_s));
	fstatus status;
    uint32_t fallback_count = 0;
	do {
		status = fat_dir_read(&dir, info);
				
		if (status == FSTATUS_OK) {
			//printf("%s\n", info->name);
            directory.entries++;
		}

        if (fallback_count++ > MAX_FILES_PER_DIR) {
            goto err;
        }

	} while (status != FSTATUS_EOF);

    directory.fd = index;
err:
    free(info);
    return directory;
}

int f32_compat_dir_close(dir_t dir) {
    if (fat_dir_close(&(f32_compat_open_dirs[dir.fd].dir)) != FSTATUS_OK)
        return -1;

    f32_compat_open_dirs[dir.fd].in_use = 0;
    
    return 0;
}

int f32_compat_register_partition(const char* path, uint32_t lba) {
    for (int index = 0; index < MAX_FAT32_PARTITIONS; index++) {
        if (f32_compat_partitions[index] == 0) {
            f32_compat_partitions[index] = register_fat32_partition(path, lba);
            return index;
        }
    }

    return -1;
}

uint8_t f32_compat_unregister_partition(int partition) {
    if (partition < 0 || partition >= MAX_FAT32_PARTITIONS)
        return 1;
    
    if (f32_compat_partitions[partition] == 0)
        return 1;

    unregstr_fat32_partition(f32_compat_partitions[partition]);
    f32_compat_partitions[partition] = 0;
    return 0;
}

uint8_t f32_compat_detect(const char* path, uint32_t lba) {
    return fat_search(path, lba);
}

int f32_compat_flush(int partition) {
    (void)partition;
    return 0;
}

struct vfs_compatible fat32_register = {
    .name = "FAT32",

    .register_partition = f32_compat_register_partition,
    .unregister_partition = f32_compat_unregister_partition,
    .detect = f32_compat_detect,
    .flush = f32_compat_flush,

    .dir_open = f32_compat_dir_open,
    .dir_close = f32_compat_dir_close
};

struct vfs_compatible * fat32_registrar = &fat32_register;

#endif