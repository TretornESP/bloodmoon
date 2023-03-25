#include "vfs.h"

#include "../memory/heap.h"

#include "../util/printf.h"
#include "../util/string.h"
#include "../util/dbgprinter.h"

#include "partition/gpt.h"
#include "partition/mbr.h"

uint16_t vfs_root_size = 0;
struct devmap * vfs_root;
struct file_system_type * file_system_type_list_head;
struct mount * mount_list_head;

struct mount * init_mount_header() {
    struct mount * mount = malloc(sizeof(struct mount));
    memset((void*)mount, 0, sizeof(struct mount));
    return mount;
}

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

void dump_mounts() {
    struct mount * mount = mount_list_head;
    while (mount != 0 && mount->device != 0 && mount->fst != 0 && mount->partition != 0) {
        printf("[%s] Dev: %s FS: %s II: %d\n", mount->partition->name, mount->device->name, mount->fst->name, mount->internal_index);
        mount = mount->next;
    }
}

void add_partition(struct partition * head, uint32_t lba, uint32_t size, uint8_t status, uint8_t type) {
    if (head == 0) {
        panic("[VFS] Invalid partition header");
    } else {
        struct partition * current = head;
        while (current->next != 0) {
            current = current->next;
        }
        current->next = init_partition_header();
        if (current->next == 0) panic("[VFS] Partition allocation failed!\n");
        current->lba = lba;
        current->size = size;
        current->status = status;
        current->type = type;
    }
}

void register_filesystem(struct vfs_compatible * registrar) {

    if (file_system_type_list_head == 0) file_system_type_list_head = init_file_system_type_header();
    
    struct file_system_type * fst = file_system_type_list_head;

    while(fst->next != 0) {
        if (!strcmp(fst->name, registrar->name))
            panic("[VFS] File system type with same name already registered\n");
        fst = fst->next;
    }

    fst->register_partition = registrar->register_partition;
    fst->unregister_partition = registrar->unregister_partition;
    fst->detect = registrar->detect;
    fst->flush = registrar->flush;

    fst->file_open = registrar->file_open;
    fst->file_close = registrar->file_close;
    fst->file_creat = registrar->file_creat;
    fst->file_read = registrar->file_read;
    fst->file_write = registrar->file_write;
    fst->file_seek = registrar->file_seek;
    fst->file_stat = registrar->file_stat;
    fst->dir_open = registrar->dir_open;
    fst->dir_close = registrar->dir_close;
    fst->dir_read = registrar->dir_read;
    fst->dir_creat = registrar->dir_creat;
    fst->rename = registrar->rename;
    fst->remove = registrar->remove;
    fst->chmod = registrar->chmod;

    fst->debug = registrar->debug;

    //TODO: Expand to vfs_compat
    if (strlen(registrar->name) > VFS_COMPAT_FS_NAME_MAX_LEN) {
        printf("[VFS] Either FS name is too long or you are tryna hack us\n");
        printf("[VFS] anyway, im restricting it to %d chars\n", VFS_COMPAT_FS_NAME_MAX_LEN);
        strncpy(fst->name, registrar->name, VFS_COMPAT_FS_NAME_MAX_LEN);
    } else {
        strncpy(fst->name, registrar->name, strlen(registrar->name));
    }
    fst->next = init_file_system_type_header();
    if (fst->next == 0) panic("[VFS] File system type allocation failed!\n");
    printf("[VFS] Registered file system type %s\n", fst->name);
}

void add_mount(struct mount * head, struct device* device, struct partition * part, struct file_system_type* fst, int internal_index) {
    if (device == 0) panic("[VFS] Invalid device\n");
    if (part == 0) panic("[VFS] Invalid partition\n");
    if (fst == 0) panic("[VFS] Invalid fs\n");
    if (head == 0) {
        panic("[VFS] Invalid mount header");
    } else {
        struct mount * current = head;
        while (current->next != 0) {
            current = current->next;
        }
        current->next = init_mount_header();
        if (current->next == 0) panic("[VFS] Mount allocation failed!\n");
        
        current->device = device;
        current->partition = part;
        current->fst = fst;
        current->internal_index = internal_index;
    }
}

uint8_t mount_fs(struct device* dev, struct partition* partition, const char* mountpoint) {

    if (mount_list_head == 0) mount_list_head = init_mount_header();
    struct file_system_type * fst = file_system_type_list_head;
    if (fst == 0) {
        printf("[VFS] There are no registerd fs -\\_(-.-)_/-\n");
        return 0;
    }

    int fs_idx = 0;
    while (fst->next != 0) {
        if (fst->detect(dev->name, partition->lba)) {
            printf("[VFS] Detected %s on %s, trying to mount on %s\n", fst->name, dev->name, mountpoint);
            int ret = fst->register_partition(dev->name, partition->lba, mountpoint);
            if (ret == -1) {
                printf("[VFS] Failed to mount %s on %s\n", fst->name, mountpoint);
                return 0;
            }
            snprintf(partition->name, 48, "%s", mountpoint);
            add_mount(mount_list_head, dev, partition, fst, ret);
            printf("[VFS] Mounted %s on %s index %d\n", fst->name, mountpoint, ret);
            return 1;
        }
        fst = fst->next;
        fs_idx++;
    }
    
    printf("[VFS] Unknown fs, mount is impossible!!!!\n");
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
        devmap->partition_no = partitions;
        if (partitions != 0 && devmap->partitions == 0)
            panic("[VFS] partitions detected but no partition struct found\n");
        if (partitions == 0)
            printf("[VFS] Skipping device %s, no partitions found\n", dev->name);
        struct partition* part = devmap->partitions;
        while (part->next != 0) {
            printf("[VFS] Partitions: %d, %d, %d, %d\n", part->lba, part->size, part->status, part->type);
            part = part->next;
        }

        dev = get_next_device(dev);
    }
}

char* get_full_path_from_fd(int fd) {
    struct file_descriptor_entry * entry = vfs_compat_get_file_descriptor(fd);
    if (entry == 0) return 0;
    char * full_path = malloc(strlen(entry->name) + strlen(entry->mount) + 1);
    if (full_path == 0) return 0;
    strcpy(full_path, entry->mount);
    strcpy(full_path + strlen(entry->mount), entry->name);
    return full_path;
}

struct mount* get_mount_from_path(const char* path, char* native_path) {
    struct mount * mount = mount_list_head;
    while (mount != 0 && mount->device != 0 && mount->fst != 0 && mount->partition != 0) {
        uint32_t mountpoint_len = strlen(mount->partition->name);
        if (strncmp(mount->partition->name, path, mountpoint_len) == 0) {
            memcpy(native_path, path + mountpoint_len, strlen(path) - mountpoint_len);
            native_path[strlen(path) - mountpoint_len] = 0;
            return mount;
        }
        mount = mount->next;
    }
    return 0;
}

void detect_partition_fs() {
    
    for (uint32_t i = 0; i < vfs_root_size; i++) {
        struct partition * current_partition = vfs_root[i].partitions;
        uint32_t partition_index = 0;
        char mountpoint[48];
        while(current_partition->next != 0) {
            memset(mountpoint, 0, 48);
            snprintf(mountpoint, 48, "%sp%d", vfs_root[i].dev->name, partition_index);

            mount_fs(vfs_root[i].dev, current_partition, mountpoint);

            current_partition = current_partition->next;
            partition_index++;
        }
    }
}

void init_vfs() {
    printf("### VFS STARTUP ###\n");
    detect_devices();
    detect_partition_fs();
    dump_mounts();
    printf("### VFS STARTUP END ###\n");
}