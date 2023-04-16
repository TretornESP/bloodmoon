#include "tty_interface.h"
#include "../../dev/devices.h"
#include "tty_dd.h"

uint64_t tty_read(const char * device, uint8_t * buffer, uint32_t size) {
    return device_read(device, size, 0, buffer);
}

uint64_t tty_write(const char * device, uint8_t * buffer, uint32_t size) {
    return device_write(device, size, 0, buffer);
}

uint64_t tty_write_now(const char * device, const char * buffer, uint32_t size) {
    uint64_t res = device_write(device, size, 0, (uint8_t*)buffer);
    device_ioctl(device, TTY_FLUSH, 0);
    return res;
}

uint64_t tty_ioctl(const char * device, uint32_t op, void * buffer) {
    return device_ioctl(device, op, buffer);
}