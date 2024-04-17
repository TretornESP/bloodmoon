#include "tty_dd.h"
#include "tty.h"
#include "../../memory/heap.h"
#include "../../dev/devices.h"
#include "../../util/printf.h"
#include "../../util/string.h"

uint64_t tty_dd_read_block_direct(struct tty* dev, uint64_t size, uint64_t skip, uint8_t* buffer) {
    char * skip_buffer = malloc(skip);
    if ((uint64_t)(int)skip != skip) {
        printf("Warning: Skipping more than 2^32 bytes in _tty_read\n");
    }

    _tty_read(dev, skip_buffer, (int)skip);
    free(skip_buffer);
    if ((uint64_t)(int)size != size) {
        printf("Warning: Reading more than 2^32 bytes in _tty_read\n");
    }
    
    _tty_read(dev, (char*)buffer, (int)size);
    return strlen((char*)buffer);
}

uint64_t tty_dd_write_block_direct(struct tty* dev, uint64_t size, uint64_t skip, uint8_t* buffer) {
    buffer += skip;
    if ((uint64_t)(int)size != size) {
        printf("Warning: Writing more than 2^32 bytes in _tty_write\n");
    }
    _tty_write(dev, (char*)buffer, (int)size);

    return size;
}

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
            _tty_add_subscriber(device, (void (*)(void*, uint8_t))data);
            return 1;
        }
        case TTY_REMOVE_SUBSCRIBER: {
            _tty_remove_subscriber(device, (void (*)(void*, uint8_t))data);
            return 1;
        }
        case TTY_MODE_RAW: {
            _tty_modes(device, _TTY_MODE_RAW, *(int*)data);
            return 1;
        }
        case TTY_MODE_ECHO: {
            _tty_modes(device, _TTY_MODE_ECHO, *(int*)data);
            return 1;
        }
        case TTY_FLUSH: {
            _tty_flush(device);
            return 1;
        }
        case TTY_VALIDATE: {
            return is_valid_tty(device);
        }
        case TTY_GET_SIZE: {
            return _tty_get_size(device);
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
   register_block(DEVICE_TTY, TTY_DD_NAME, &tty_fops);
}