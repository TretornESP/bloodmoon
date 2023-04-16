#include "tty.h"
#include "../../../memory/heap.h"
#include "../../../util/string.h"
#include "../../../util/printf.h"
#include "../../../util/dbgprinter.h"
#include "../../../drivers/serial/serial.h"

int tty_id = 0;


struct tty* tty_register_device(const char * device, const char * mountpoint, uint8_t mode) {
    if (strncmp(device, "/dev/com", 8)) return;
    if (mode != TTY_MODE_RAW && mode != TTY_MODE_CANONICAL) return;

    struct tty* tty = (struct tty*)calloc(1, sizeof(struct tty));
    if (!tty) return 0;

    int comm = atoi(device + 8);
    struct serial_device * serial_dev = get_serial_by_comm(comm);

    tty->com_port = serial_dev->port;
    tty->mode = mode;
    tty->id = tty_id++;
    snprintf(tty->name, 32, "%s", mountpoint);

    printf("TTY: Registering device %s on port %d\n", device, serial_dev->port);

    return tty;
}
void tty_unregister_device(struct tty* tty) {}
uint8_t tty_search(uint32_t port) {}
int tty_sync(struct tty* device) {}
void tty_dump_device(struct tty* device) {}

uint64_t vfs_tty_get_size(struct tty* device) {(void)device; return 0;}
uint8_t vfs_tty_read(struct tty * device, uint8_t * destination_buffer, uint64_t size, uint64_t skip) {

    struct serial_device* serial = get_serial(device->com_port);
    (void)serial;

    return 0;
}
uint8_t vfs_tty_write(struct tty * device, uint8_t * source_buffer, uint64_t size, uint64_t skip) {

    return 0;
}