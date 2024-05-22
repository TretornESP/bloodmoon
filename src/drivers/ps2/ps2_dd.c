#include <stdint.h>
#include "ps2.h"
#include "ps2_dd.h"
#include "../../util/string.h"
#include "../../devices/devices.h"

uint64_t mouse_dd_read(uint64_t port, uint64_t size, uint64_t skip, uint8_t* buffer) {
    (void)port;
    (void)size;
    (void)skip;
    struct ps2_mouse_status mouse;
    if (read_mouse(&mouse)) {
        memcpy(buffer, &mouse, sizeof(struct ps2_mouse_status));
        return 1;
    } else {
        return 0;
    }
}

uint64_t mouse_dd_write(uint64_t port, uint64_t size, uint64_t skip, uint8_t* buffer) {
    (void)port;
    (void)size;
    (void)skip;
    (void)buffer;
    return 0;
}

uint64_t mouse_dd_ioctl(uint64_t port, uint32_t op, void* data) {
    (void)port;
    switch (op) {
        case IOCTL_MOUSE_SUBSCRIBE: {
            ps2_subscribe(data, PS2_DEVICE_MOUSE, PS2_DEVICE_GENERIC_EVENT);
            return 1;
        }
        case IOCTL_MOUSE_UNSUBSCRIBE: {
            ps2_unsubscribe(data);
            return 1;
        }
        case IOCTL_MOUSE_SUBSCRIBE_EVENT: {
            ps2_subscribe(data, PS2_DEVICE_MOUSE, PS2_DEVICE_SPECIAL_EVENT);
            return 1;
        }
        case IOCTL_MOUSE_GET_STATUS: {
            struct ps2_mouse_status mouse;
            if (read_mouse(&mouse)) {
                memcpy(data, &mouse, sizeof(struct ps2_mouse_status));
                return 1;
            } else {
                return 0;
            }
        }
        default: {
            return 0;
        }
    }

    return 0;
}

uint64_t keyboard_dd_read(uint64_t port, uint64_t size, uint64_t skip, uint8_t* buffer) {
    (void)port;
    (void)size;
    (void)skip;
    char key;
    if (read_keyboard(&key)) {
        buffer[0] = key;
        return 1;
    } else {
        return 0;
    }
}

uint64_t keyboard_dd_write(uint64_t port, uint64_t size, uint64_t skip, uint8_t* buffer) {
    (void)port;
    (void)size;
    (void)skip;
    (void)buffer;
    return 0;
}

uint64_t keyboard_dd_ioctl(uint64_t port, uint32_t op, void* data) {
    (void)port;
    switch (op) {
        case IOCTL_KEYBOARD_SUBSCRIBE: {
            ps2_subscribe(data, PS2_DEVICE_KEYBOARD, PS2_DEVICE_GENERIC_EVENT);
            return 1;
        }
        case IOCTL_KEYBOARD_UNSUBSCRIBE: {
            ps2_unsubscribe(data);
            return 1;
        }
        case IOCTL_KEYBOARD_HALT_UNTIL_ENTER: {
            keyboard_halt_until_enter();
            return 1;
        }
        default: {
            return 0;
        }
    }

    return 0;
}

struct file_operations mouse_fops = {
   .read = mouse_dd_read,
   .write = mouse_dd_write,
   .ioctl = mouse_dd_ioctl
};

struct file_operations keyboard_fops = {
   .read = keyboard_dd_read,
   .write = keyboard_dd_write,
   .ioctl = keyboard_dd_ioctl
};

void init_keyboard_dd() {
    register_char(DEVICE_KEYBOARD, KEYBOARD_DD_NAME, &keyboard_fops);
}

void init_mouse_dd() {
   register_block(DEVICE_MOUSE, MOUSE_DD_NAME, &mouse_fops);
}