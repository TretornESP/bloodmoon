#include "serial_dd.h"
#include "serial.h"
#include "../../dev/devices.h"
#include "../../util/printf.h"

uint64_t serial_dd_read_block(uint64_t port, uint64_t size, uint64_t skip, uint8_t* buffer) {
    struct serial_device* device = get_serial((int)port);
    if (!device) return 0;

    for (uint64_t i = 0; i < skip; i++) {
        read_inb(device);
    }
    for (uint64_t i = 0; i < size; i++) {
        buffer[i] = read_inb(device);
        if (buffer[i] == 0) return i;
    }
    return size;
}

uint64_t serial_dd_write_block(uint64_t port, uint64_t size, uint64_t skip, uint8_t* buffer) {
    printf("Writing to serial port %x (size: %d, skip: %d)\n", port, size, skip);
    struct serial_device* device = get_serial((int)port);
    if (!device) return 0;

    buffer += skip;

    for (uint64_t i = 0; i < size; i++) {
        write_outb(device, buffer[i]);
    }

    return size;
}

uint64_t serial_dd_ioctl(uint64_t port, uint32_t op, void* data) {
    (void)data;
    
    switch (op) {
        case SERIAL_FLUSH_TX: {
            _serial_flush(port);
            return 1;
        }
        case SERIAL_SUBSCRIBE_READ: {
            serial_read_event_add(port, (void (*)(char c, int port))data);
            return 1;
        }
        case SERIAL_UNSUBSCRIBE_READ: {
            serial_read_event_remove(port, (void (*)(char c, int port))data);
            return 1;
        }
        case SERIAL_SUBSCRIBE_WRITE: {
            serial_write_event_add(port, (void (*)(char c, int port))data);
            return 1;
        }
        case SERIAL_UNSUBSCRIBE_WRITE: {
            serial_write_event_remove(port, (void (*)(char c, int port))data);
            return 1;
        }
        case SERIAL_ENABLE_ECHO: {
            serial_echo_enable(port);
            return 1;
        }
        case SERIAL_DISABLE_ECHO: {
            serial_echo_disable(port);
            return 1;
        }
        default:
            return 0;
    }
    return 0;
}

struct file_operations serial_fops = {
   .read = serial_dd_read_block,
   .write = serial_dd_write_block,
   .ioctl = serial_dd_ioctl
};

void init_serial_dd() {
   register_char(DEVICE_SERIAL, "SERIAL DRIVER", &serial_fops);
}