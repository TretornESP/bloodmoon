#include "vfs.h"
#include "../dev/devices.h"
#include "../util/printf.h"
#include "../memory/heap.h"
#include "fat32.h"

struct super_block * superblock_list_head;

void alloc_super() {

}

void init_vfs_fat(struct device* dev) {
    
}

void detect_fs() {
    uint8_t * buffer = malloc(512);
    read_device(dev->name, 1, 0, buffer);
    if (fat_search(buffer)) {
        return FAT32_FS;
    }
    return UNKNOWN_FS;
}

void init_vfs() {
    struct device* dev = get_device_head();
    while (dev != 0) {
        switch(detect_fs()) {
            case FAT32_FS:
                printf("FAT32 FS detected in %s\n", dev->name);
                init_vfs_fat(dev);
                break;
            case EXT2_FS:
                printf("EXT2 is not supported yet\n")
                break;
            case NTFS_FS:
                printf("NTFS is not supported yet\n")
                break;
            default:
                printf("Unknown filesystem on %s\n", dev->name);
                break;
        }
        dev = get_next_device(dev);
    }
}

void register_fs(const char * name, 