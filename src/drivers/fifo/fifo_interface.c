#include "fifo_interface.h"
#include "fifo_dd.h"
#include "../../devices/fifo/fifo.h"
#include "../../devices/devices.h"

char * create_fifo(uint64_t size) {
    struct fifo* fifo = fifo_alloc(size);
    if (!fifo) return (char*)0;
    return device_create((void*)0, DEVICE_FIFO, (uint64_t)fifo);
}

void destroy_fifo(const char * device) {
    struct device* fifo_dev = device_search(device);
    struct fifo* fifo = (struct fifo*)fifo_dev->internal_id;
    fifo_kfree(fifo);
    device_destroy(device);
}

uint64_t fifo_read(const char * device, uint8_t * buffer, uint32_t skip, uint32_t size) {
    return device_read(device, size, skip, buffer);
}

uint64_t fifo_write(const char * device, uint8_t * buffer, uint32_t skip, uint32_t size) {
    return device_write(device, size, skip, buffer);
}

uint64_t fifo_identify(const char* device) {
    return device_identify(device, FIFO_DD_NAME);
}

uint64_t fifo_dev_flush(const char* device) {
    return device_ioctl(device, IOCTL_FIFO_FLUSH, (void*)0);
}

uint64_t fifo_get_size(const char* device) {
    return device_ioctl(device, IOCTL_FIFO_GET_SIZE, (void*)0);
}