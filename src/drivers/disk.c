#include "disk.h"
#include "../util/string.h"
#include "../dev/ahci.h"
#include "../util/printf.h"
#include "../dev/devices.h"

uint64_t disk_read(uint8_t port, uint64_t size, uint64_t offset, uint8_t* buffer) {
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

struct file_operations disk_fops = {
    .read = disk_read
};

void init_drive(void) {
    register_block(8, "Simple SCSI Driver", &disk_fops);
}

void exit_drive(void) {}