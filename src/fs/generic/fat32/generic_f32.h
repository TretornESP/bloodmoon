#ifndef _GENERIC_F32_H
#define _GENERIC_F32_H
#include "../vfs_compat.h"
#include "fat32.h"
#define MAX_OPEN_DIRS 256
#define MAX_REQUESTED_FILES 256
#define MAX_FILES_PER_DIR 4096
struct f32_dir_usage {
    int in_use;
    struct dir_s dir;
};
struct f32_dir_usage f32_compat_open_dirs[MAX_OPEN_DIRS];

struct info_s requested_files[MAX_REQUESTED_FILES];
int last_open_file = 0;

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

int f32_compat_dir_read(dir_t dir) {

}

dir_t f32_compat_dir_create(const char* dname) {
    
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