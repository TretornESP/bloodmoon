#ifndef _GENERIC_TTY_H
#define _GENERIC_TTY_H

#include "tty.h"

#include "../vfs_compat.h"

#include "../../../util/printf.h"
#include "../../../util/dbgprinter.h"

struct tty* tty_devices[MAX_TTY_DEVICES] = {0};
//-1 cannot return the object
// 0 all okey
// 1 error

void tty_read_callback(char c, int port) {
    for (int i = 0; i < MAX_TTY_DEVICES; i++) {
        if (tty_devices[i] != 0) {
            if (tty_devices[i]->com_port == port) {
                tty_buffer_write(tty_device // todo: 08/04/2023
                if (tty_devices[i]->signal_handler != 0) {
                    tty_devices[i]->signal_handler(TTY_SIGNAL_READ);
                }
            }
        }
    }
}

int tty_compat_register_device(const char* device, uint32_t mode, const char* mountpoint) {
    for (int i = 0; i < MAX_TTY_DEVICES; i++) {
        if (tty_devices[i] == 0) {
            tty_devices[i] = tty_register_device(structdevice, mountpoint, mode);
            if (tty_devices[i] == 0) {
                return -1;
            }
            
            return i;
        }
    }
    return -1;
}

uint8_t tty_compat_unregister_device(int index) {
    if (tty_devices[index] != 0) {
        tty_unregister_device(tty_devices[index]);
        tty_devices[index] = 0;
        return 0;
    }
    return 1;
}

uint8_t tty_compat_detect(const char* dummy , uint32_t port) {
    (void*)dummy;
    return tty_search(port);
}

int tty_compat_flush(int index) {
    if (index < 0 || index >= MAX_TTY_DEVICES)
        return -1;

    struct tty* device = tty_devices[index];
    return (int)tty_sync(device);
}

void tty_compat_debug() {
    for (int i = 0; i < MAX_TTY_DEVICES; i++) {
        if (tty_devices[i] != 0) {
            printf("Tty %d\n", i);
            tty_dump_device(tty_devices[i]);
        }
    }
}

uint64_t tty_compat_file_read(int devno, int fd, void* buffer, uint64_t size) {
    if (partno < 0 || partno >= MAX_TTY_DEVICES) 
        return -1;

    struct tty * device = tty_devices[devno];
    if (device == 0)
        return -1;

    struct device_descriptor_entry * entry = vfs_compat_get_device(fd);
    if (entry == 0 || entry->loaded == 0) return -1;
    return vfs_tty_read(device, buffer, size, entry->offset);
}

uint64_t tty_compat_file_write(int devno, int fd, void* buffer, uint64_t size) {
    if (devno < 0 || devno >= MAX_TTY_DEVICES) 
        return -1;

    struct tty * device = tty_devices[devno];
    if (device == 0)
        return -1;

    struct device_descriptor_entry * entry = vfs_compat_get_device(fd);
    if (entry == 0 || entry->loaded == 0) return -1;
    return vfs_tty_write(device, buffer, size, entry->offset);
}

uint64_t tty_compat_file_seek(int devno, int fd, uint64_t offset, int whence) {
    if (devno < 0 || devno >= MAX_TTY_DEVICES) 
        return -1;

    struct tty * device = tty_devices[devno];
    if (device == 0)
        return -1;

    struct device_descriptor_entry * entry = vfs_compat_get_device(fd);
    if (entry == 0 || entry->loaded == 0) return -1;

    uint64_t size = 0;
    switch (whence) {
        case SEEK_SET: {
            entry->offset = offset;
            break;
        }
        case SEEK_CUR: {
            entry->offset += offset;
            break;
        }
        case SEEK_END: {
            size = vfs_tty_get_size(device);
            entry->offset = size + offset;
            break;
        }
        default:
            return 0;
    }

    return entry->offset;
}

int tty_compat_file_open(int devno, const char* path, int flags, int mode) {
    if (devno < 0 || devno >= MAX_TTY_DEVICES) 
        return -1;

    struct tty * device = tty_devices[devno];
    if (device == 0)
        return -1;

    if (!strcmp(path, "/"))
        return -1;

    return get_devfd(path, device->name, flags, mode);
}

int tty_compat_file_close(int devno, int fd) {
    if (partno < 0 || partno >= MAX_TTY_DEVICES) 
        return -1;

    struct tty * device = tty_devices[devno];
    if (device == 0)
        return -1;

    return release_devfd(fd);
}

int tty_compat_dir_open(int partno, const char* path) {return -1;}
int tty_compat_dir_close(int partno, int fd) {return -1;}
int tty_compat_file_creat(int partno, const char* path, int mode) {return -1;}
int tty_compat_dir_creat(int partno, const char* path, int mode) {return -1;}
int tty_compat_dir_read(int partno, int fd, char* name, uint32_t * name_len, uint32_t * type) {return -1;}
int tty_compat_dir_load(int partno, int fd) {return -1;}
int tty_compat_stat(int partno, int fd, stat_t* st) {return -1;}
int tty_compat_rename(int partno, const char* path, const char* newpath) {return -1;}
int tty_compat_prepare_remove(int partno, const char* path) {return -1;}
int tty_compat_remove(int partno, const char* path) {return -1;}
int tty_compat_chmod(int partno, const char* path, int mode) {return -1;}

struct vfs_compatible tty_register = {
    .name = "TTY",
    .register_partition = tty_compat_register_partition,
    .unregister_partition = tty_compat_unregister_partition,
    .detect = tty_compat_detect,
    .flush = tty_compat_flush,
    .debug = tty_compat_debug,
    .file_open = tty_compat_file_open,
    .file_close = tty_compat_file_close,
    .file_creat = tty_compat_file_creat,
    .file_read = tty_compat_file_read,
    .file_write = tty_compat_file_write,
    .file_seek = tty_compat_file_seek,
    .file_stat = tty_compat_stat,
    .rename = tty_compat_rename,
    .remove = tty_compat_remove,
    .chmod = tty_compat_chmod,
    .dir_open = tty_compat_dir_open,
    .dir_close = tty_compat_dir_close,
    .dir_creat = tty_compat_dir_creat,
    .dir_read = tty_compat_dir_read,
    .dir_load = tty_compat_dir_load,
    .prepare_remove = tty_compat_prepare_remove
};

struct vfs_compatible * tty_registrar = &tty_register;

#endif