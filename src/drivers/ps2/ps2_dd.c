#include <stdint.h>
#include "ps2.h"
#include "ps2_dd.h"
#include "../../util/string.h"
#include "../../dev/devices.h"

uint64_t mouse_dd_read(uint64_t port, uint64_t size, uint64_t skip, uint8_t* buffer) {
    (void)port;
    (void)size;
    (void)skip;
    struct ps2_mouse_status * mouse = read_mouse();
    if (mouse) {
        memcpy(buffer, mouse, sizeof(struct ps2_mouse_status));
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
    (void)op;
    (void)data;
    return 0;
}

struct file_operations mouse_fops = {
   .read = mouse_dd_read,
   .write = mouse_dd_write,
   .ioctl = mouse_dd_ioctl
};

void init_mouse_dd() {
   register_block(DEVICE_MOUSE, MOUSE_DD_NAME, &mouse_fops);
}