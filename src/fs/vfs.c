#include "vfs.h"
#include "../dev/devices.h"
#include "../util/printf.h"
#include "../memory/heap.h"
#include "fat32.h"
#include "../util/string.h"
#include "../util/dbgprinter.h"
#include "gpt.h"
#include "mbr.h"

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

//This is not the best way, perhaps we could register fs on demand from
//kernel.c
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
    panic("Trying to init fat32\n");
}

uint8_t detect_fs(struct device* dev, struct partition* partition) {
    uint8_t buffer[512];
    if (!disk_read(dev->name, buffer, partition->lba, 1)) return INVALID_FS;

    if (fat_search(buffer)) {
        return FAT32_FS;
    }
    return UNKNOWN_FS;
}

uint32_t detect_partitions(struct device* dev, struct partition* part) {
    printf("Calling print part\n");
    uint32_t partition_number = read_gpt(dev->name, part);
    if (partition_number == 0) {
        printf("No gpt partitions found, trying mbr\n");
        partition_number = read_mbr(dev->name, part);
    }
    return partition_number;
}

void init_vfs() {
    struct device* dev = get_device_head();

    while (dev != 0) {
        printf("Device: %s\n", dev->name);
        struct partition* part;
        memset(part, 0, sizeof(struct partition) * 4);
        uint32_t partitions = detect_partitions(dev, part);
        for (uint32_t i = 0; i < partitions; i++) {
            uint8_t type = detect_fs(dev, &part[i]);
            if (type == INVALID_FS) {
                printf("Invalid filesystem in %s part: %d. SKIPPING!\n", dev->name, i);
                continue;
            }
            if (type != UNKNOWN_FS) {
                printf("Registering new filesystem of type: %d, on %s part: %i\n", type, dev->name, i);
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
        }
        dev = get_next_device(dev);
    }
}

void register_fs() {}