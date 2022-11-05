#include "vfs.h"
#include "../util/printf.h"
#include "../memory/heap.h"
#include "fat32.h"
#include "../util/string.h"
#include "../util/dbgprinter.h"
#include "vfs_adapters.h"
#include "gpt.h"
#include "mbr.h"

struct devmap * vfs_root;
struct file_system_type * file_system_type_list_head;

void add_partition(struct partition * head, uint32_t lba, uint32_t size, uint8_t status, uint8_t type) {
    struct partition * new_partition = (struct partition*)malloc(sizeof(struct partition));
    new_partition->lba = lba;
    new_partition->size = size;
    new_partition->status = status;
    new_partition->type = type;
    new_partition->next = 0;

    if (head == 0) {
        head = new_partition;
    } else {
        struct partition * current = head;
        while (current->next != 0) {
            current = current->next;
        }
        current->next = new_partition;
    }
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
            fst->next->mount = vfs_f32_mount;
            fst->next->kill_sb = vfs_f32_kill_sb;
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

uint8_t detect_fs(struct device* dev, struct partition* partition) {
    uint8_t buffer[512];
    if (!disk_read(dev->name, buffer, partition->lba, 1)) return INVALID_FS;

    if (fat_search(buffer)) {
        return FAT32_FS;
    }
    return UNKNOWN_FS;
}

uint32_t detect_partitions(struct device* dev, struct partition* part) {
    uint32_t partition_number = read_gpt(dev->name, part, add_partition);
    if (partition_number == 0) {
        printf("No gpt partitions found, trying mbr\n");
        partition_number = read_mbr(dev->name, part, add_partition);
    }
    return partition_number;
}

void init_vfs() {
    uint32_t device_count = get_device_count();
    vfs_root = (struct devmap*)malloc(sizeof(struct devmap)*device_count);
    
    uint32_t device_index = 0;
    struct device* dev = get_device_head();
    
    while (dev != 0) {
        struct devmap* devmap = &vfs_root[device_index++];
        devmap->dev = dev;
        devmap->partitions = 0x0;
        printf("Device: %s\n", dev->name);

        uint32_t partitions = detect_partitions(dev, devmap->partitions);
        if (partitions != 0 && devmap->partitions == 0)
            panic("Partitions detected but no partition struct found\n");

        struct partition* part = devmap->partitions;
        while (part != 0) {
            printf("Partition: %d, %d, %d, %d\n", part->lba, part->size, part->status, part->type);
            part = part->next;
        }
        /*
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
        }*/

        dev = get_next_device(dev);
    }
}

void register_fs() {}