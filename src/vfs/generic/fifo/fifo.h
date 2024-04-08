#ifndef _VFS_FIFO_H
#define _VFS_FIFO_H

#include <stdint.h>
#include "../../../drivers/serial/serial.h"

struct vfs_fifo {
    char name[32];
    char device[32];
};

struct vfs_fifo* fifo_register_device(const char * device, uint32_t mode, const char * mountpoint);
void fifo_unregister_device(struct vfs_fifo* fifo);
uint8_t fifo_search(const char* name);
int fifo_sync(struct vfs_fifo* fifo);
void fifo_dump_device(struct vfs_fifo* fifo);
uint64_t vfs_fifo_get_size(struct vfs_fifo* fifo);
uint8_t vfs_fifo_read(struct vfs_fifo * fifo, uint8_t * destination_buffer, uint64_t size, uint64_t skip);
uint8_t vfs_fifo_write(struct vfs_fifo * fifo, uint8_t * source_buffer, uint64_t size, uint64_t skip);

#endif