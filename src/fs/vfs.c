#include "vfs.h"

#include "../memory/heap.h"

#include "../util/printf.h"
#include "../util/string.h"
#include "../util/dbgprinter.h"

#include "../drivers/disk/disk_interface.h"
#include "partition/gpt.h"
#include "partition/mbr.h"

uint16_t vfs_root_size = 0;
struct devmap * vfs_root;
struct file_system_type * file_system_type_list_head;

struct partition * init_partition_header() {
    struct partition * part = malloc(sizeof(struct partition));
    memset((void*)part, 0, sizeof(struct partition));
    return part;
}

struct file_system_type * init_file_system_type_header() {
    struct file_system_type * fst = malloc(sizeof(struct file_system_type));
    memset((void*)fst, 0, sizeof(struct file_system_type));
    return fst;
}

void add_partition(struct partition * head, uint32_t lba, uint32_t size, uint8_t status, uint8_t type) {
    if (head == 0) {
        panic("Invalid partition header");
    } else {
        struct partition * current = head;
        while (current->next != 0) {
            current = current->next;
        }
        current->next = init_partition_header();
        if (current->next == 0) panic("Partition allocation failed!\n");
        current->lba = lba;
        current->size = size;
        current->status = status;
        current->type = type;
    }
}

void register_filesystem_type(struct vfs_compatible * registrar) {

    if (file_system_type_list_head == 0) file_system_type_list_head = init_file_system_type_header();
    
    struct file_system_type * fst = file_system_type_list_head;

    while(fst->next != 0) {
        if (!strcmp(fst->name, registrar->name))
            panic("File system type with same name already registered\n");
        fst = fst->next;
    }

    fst->register_partition = registrar->register_partition;
    fst->unregister_partition = registrar->unregister_partition;
    fst->detect = registrar->detect;
    if (strlen(registrar->name) > VFS_COMPAT_FS_NAME_MAX_LEN) {
        printf("Either FS name is too long or you are tryna hack us\n");
        printf("anyway, im restricting it to %d chars\n", VFS_COMPAT_FS_NAME_MAX_LEN);
        strncpy(fst->name, registrar->name, VFS_COMPAT_FS_NAME_MAX_LEN);
    } else {
        strncpy(fst->name, registrar->name, strlen(registrar->name));
    }
    fst->next = init_file_system_type_header();
    if (fst->next == 0) panic("File system type allocation failed!\n");
    printf("[VFS] Registered file system type %s\n", fst->name);
}

uint8_t mount_fs(struct device* dev, struct partition* partition) {
    uint8_t buffer[512];
    if (!disk_read(dev->name, buffer, partition->lba, 1)) return 0;

    struct file_system_type * fst = file_system_type_list_head;
    if (fst == 0) {
        printf("There are no registerd fs -\\_(-.-)_/-\n");
        return 0;
    }

    while (fst->next != 0) {
        printf("Searching for %s fs in partion\n", fst->name);
        if (fst->detect(buffer)) {
            printf("Its a match for %s\n", fst->name);
            return 1;
        }
    }
    
    printf("Unknown fs, mount is impossible!!!!\n");
    return 0;
}

uint32_t detect_partitions(struct device* dev, struct partition* part) {
    uint32_t partition_number = read_gpt(dev->name, part, add_partition);
    if (partition_number == 0) {
        partition_number = read_mbr(dev->name, part, add_partition);
    }
    return partition_number;
}

void detect_devices() {
    vfs_root_size = get_device_count(); //TODO: Limit this to drives only!
    vfs_root = (struct devmap*)malloc(sizeof(struct devmap)*vfs_root_size);
    
    uint32_t device_index = 0;
    struct device* dev = get_device_head();
    
    while (dev != 0) {
        struct devmap* devmap = &vfs_root[device_index++];
        devmap->dev = dev;
        devmap->partitions = init_partition_header();
        printf("[VFS] Scanning device: %s\n", dev->name);

        uint32_t partitions = detect_partitions(dev, devmap->partitions);
        if (partitions != 0 && devmap->partitions == 0)
            panic("partitions detected but no partition struct found\n");

        struct partition* part = devmap->partitions;
        while (part->next != 0) {
            printf("[VFS] Partitions: %d, %d, %d, %d\n", part->lba, part->size, part->status, part->type);
            part = part->next;
        }

        dev = get_next_device(dev);
    }
}

void detect_partition_fs() {
    
    for (uint32_t i = 0; i < vfs_root_size; i++) {
        struct partition * current_partition = vfs_root[i].partitions;
        while(current_partition->next != 0) {
            
            mount_fs(vfs_root[i].dev, current_partition);

            current_partition = current_partition->next;
        }
    }
}

void init_vfs() {
    printf("### VFS STARTUP ###\n");
    detect_devices();
    detect_partition_fs();
}