#include "vfs.h"
#include "../dev/devices.h"
#include "../util/printf.h"
#include "../memory/heap.h"
#include "fat32.h"
#include "../util/string.h"
#include "../util/dbgprinter.h"

struct filesystem * filesystem_list_head;
struct file_system_type * file_system_type_list_head;

struct dentry *fat32_mount(struct file_system_type * fs_type, int a, const char * b, void *c) {}
void fat32_kill_sb(struct super_block * sb) {}

void alloc_fs() {
    struct filesystem * fs = filesystem_list_head;
    if (fs == 0) fs = calloc(0, sizeof(struct filesystem));

    while(fs->next != NULL) {
        fs = fs->next;
    }

    fs->next = malloc(sizeof(struct filesystem));
    //TODO
}

void register_filesystem_type(uint8_t type) {

    struct file_system_type * fst = file_system_type_list_head;
    if (fst == 0) fst = calloc(0, sizeof(struct file_system_type));

    while(fst->next != 0) {
        if (fst->type == type) return;
        fst = fst->next;
    }

    switch(type) {
        case FAT32_FS:
            fst->next = malloc(sizeof(struct file_system_type));
            fst->type = type;
            fst->next->mount = fat32_mount;
            fst->next->kill_sb = fat32_kill_sb;
            fst->next->next = 0;
            memcpy(fst->next->name, "FAT32", 5);
            break;
        case EXT2_FS:
            printf("EXT2 is not supported yet\n");
            break;
        case NTFS_FS:
            printf("NTFS is not supported yet\n");
            break;
        default:
            printf("Unknown filesystem\n");
            break;
    }
}

//TODO: GENERALIZE THIS
void init_vfs_fat(struct device* dev) {

}

uint8_t detect_fs(struct device* dev) {
    uint8_t buffer[512];
    device_read(dev->name, 1, 0, buffer);

    if (fat_search(buffer)) {
        return FAT32_FS;
    }
    return UNKNOWN_FS;
}

void init_vfs() {
    struct device* dev = get_device_head();

    while (dev != 0) {
        printf("Device: %s\n", dev->name);
        uint8_t type = detect_fs(dev);
        if (type != UNKNOWN_FS) {
            printf("Registering filesystem on %s\n", dev->name);
            register_filesystem_type(type);
        }
        switch(type) {
            case FAT32_FS:
                printf("FAT32 FS detected in %s\n", dev->name);
                init_vfs_fat(dev);
                break;
            case EXT2_FS:
                printf("EXT2 is not supported yet\n");
                break;
            case NTFS_FS:
                printf("NTFS is not supported yet\n");
                break;
            default:
                printf("Unknown filesystem on %s\n", dev->name);
                break;
        }
        dev = get_next_device(dev);
    }
}

void register_fs() {}