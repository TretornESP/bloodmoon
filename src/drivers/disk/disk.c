#include "disk.h"
#include "../ahci/ahci.h"
#include "../../util/string.h"
#include "../../util/printf.h"
#include "../../util/dbgprinter.h"
#include "../../dev/devices.h"

uint64_t dd_disk_read(uint8_t port, uint64_t size, uint64_t offset, uint8_t* buffer) {
    uint8_t * hw_buffer = get_buffer(port);
    for (uint64_t i = 0; i < size; i++) {
        if (read_port(port, offset+i, 1)) {
            memcpy(buffer + (512*i), hw_buffer, 512);
        } else {
            return i;
        }
    }
    return size;
}

uint64_t dd_disk_write(uint8_t port, uint64_t size, uint64_t offset, uint8_t* buffer) {
    uint8_t * hw_buffer = get_buffer(port);
    
    for (uint64_t i = 0; i < size; i++) {
        memcpy(hw_buffer, buffer + (512*i), 512);
        if (!write_port(port, offset+i, 1)) {
            return i;
        }
    }
    return size;
}

uint64_t dd_disk_atapi_read(uint8_t port, uint64_t size, uint64_t offset, uint8_t* buffer) {
    if (offset != 0)
        printf("WARNING: ATAPI READS ARE NOT WORKING WELL, AN OFFSET != 0 YIELDS ZEROS\n");
    uint8_t * hw_buffer = get_buffer(port);
    if (!read_atapi_port(port, offset, size))
        return 0;
    memcpy(buffer, hw_buffer, 512*size);
    return size;
}

uint64_t dd_disk_atapi_write(uint8_t port, uint64_t size, uint64_t offset, uint8_t* buffer) {
    panic("ATAPI WRITE NOT IMPLEMENTED");
    uint8_t * hw_buffer = get_buffer(port);
    
    for (uint64_t i = 0; i < size; i++) {
        memcpy(hw_buffer, buffer + (512*i), 512);
        if (!write_port(port, offset+i, 1)) {
            return i;
        }
    }
    return size;
}

uint64_t dd_ioctl (uint8_t port, uint32_t op , void* data) {
    uint8_t * hw_buffer = get_buffer(port);

    switch (op) {
        case IOCTL_ATAPI_IDENTIFY:
            identify(port);
            memcpy(data, hw_buffer, 512);
            break;
        case IOCTL_INIT:
            return 1; //TODO: Modify
        case IOCTL_CTRL_SYNC:
            return 1; //TODO: Modify if the buffer cache is implemented or async reads
        case IOCTL_CTRL_TRIM:
            return 1; //TODO: Modify if the buffer cache is implemented or async reads
        case IOCTL_GET_SECTOR_SIZE: {
            return 512;
        }
        case IOCTL_GET_SECTOR_COUNT: {
            identify(port);
            struct sata_ident * sident = (struct sata_ident*) hw_buffer;
            return sident->CurrentSectorCapacity;
        }
        case IOCTL_GET_BLOCK_SIZE:
            return 512;
    }
    return 0;
}

struct file_operations ata_fops = {
    .read = dd_disk_read,
    .write = dd_disk_write,
    .ioctl = dd_ioctl
};

struct file_operations atapi_fops = {
    .read = dd_disk_atapi_read,
    .write = dd_disk_atapi_write,
    .ioctl = dd_ioctl
};

void init_drive(void) {
    printf("### DRIVE STARTUP ###\n");
    register_block(8, "ATA DRIVER", &ata_fops);
    register_block(9, "ATAPI DRIVER", &atapi_fops);
}

void exit_drive(void) {}