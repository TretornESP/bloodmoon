#include "tty_dd.h"
#include "tty.h"
#include "../../memory/heap.h"
#include "../../dev/devices.h"
#include "../../util/printf.h"
#include "../../util/string.h"

uint64_t tty_dd_read_block(uint64_t port, uint64_t size, uint64_t skip, uint8_t* buffer) {
    struct tty* device = get_tty((int)port);
    if (!is_valid_tty(device)) return 0;

    char * skip_buffer = malloc(skip);
    if ((uint64_t)(int)skip != skip) {
        printf("Warning: Skipping more than 2^32 bytes in tty_dd_read_block\n");
    }
    _tty_read(device, skip_buffer, (int)skip);
    free(skip_buffer);
    if ((uint64_t)(int)size != size) {
        printf("Warning: Reading more than 2^32 bytes in tty_dd_read_block\n");
    }
    _tty_read(device, (char*)buffer, (int)size);
    return strlen((char*)buffer);
}

uint64_t tty_dd_write_block(uint64_t port, uint64_t size, uint64_t skip, uint8_t* buffer) {
    //printf("Writing to serial port %x (size: %d, skip: %d)\n", port, size, skip);
    struct tty* device = get_tty((int)port);
    if (!is_valid_tty(device)) return 0;

    buffer += skip;
    if ((uint64_t)(int)size != size) {
        printf("Warning: Writing more than 2^32 bytes in tty_dd_write_block\n");
    }
    _tty_write(device, (char*)buffer, (int)size);

    return size;
}

uint64_t tty_dd_ioctl(uint64_t port, uint32_t op, void* data) {
    struct tty* device = get_tty((int)port);
    if (!is_valid_tty(device)) return 0;
    
    switch (op) {
        case TTY_ADD_SUBSCRIBER: {
            tty_add_subscriber(device, (void (*)(void*, uint8_t))data);
            return 1;
        }
        case TTY_REMOVE_SUBSCRIBER: {
            tty_remove_subscriber(device, (void (*)(void*, uint8_t))data);
            return 1;
        }
        case TTY_MODE_RAW: {
            tty_modes(device, _TTY_MODE_RAW, *(int*)data);
            return 1;
        }
        case TTY_MODE_ECHO: {
            tty_modes(device, _TTY_MODE_ECHO, *(int*)data);
            return 1;
        }
        case TTY_FLUSH: {
            tty_flush(device);
            return 1;
        }
        default:
            return 0;
    }
    return 0;
}

struct file_operations tty_fops = {
   .read = tty_dd_read_block,
   .write = tty_dd_write_block,
   .ioctl = tty_dd_ioctl
};

void init_tty_dd() {
   register_block(DEVICE_TTY, "TTY DRIVER", &tty_fops);
}