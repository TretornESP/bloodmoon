#include "tty.h"
#include "../../../memory/heap.h"
#include "../../../util/string.h"
#include "../../../util/printf.h"
#include "../../../util/dbgprinter.h"
#include "../../../drivers/serial/serial.h"

struct tty* tty_register_device(const char * device, const char * mountpoint) {
    struct tty* tty = (struct tty*)calloc(1, sizeof(struct tty));
    if (!tty) return 0;

    if (strncmp(device, "/dev/com", 8)) return;

    int port = atoi(device + 8);

    printf("TTY: Registering device %s on port %d\n", device, port);

    return tty;
}
void tty_unregister_device(struct tty* tty) {}
uint8_t tty_search(uint32_t port) {}
int tty_sync(struct tty* device) {}
void tty_dump_device(struct tty* device) {}

uint64_t tty_get_size(struct tty* device) {(void*)device; return 0;}
uint8_t tty_read(struct tty * device, uint8_t * destination_buffer, uint64_t size, uint64_t skip) {

    struct serial_device* serial = get_serial(device->com_port);

    return 0;
}
uint8_t tty_write(struct tty * device, uint8_t * source_buffer, uint64_t size, uint64_t skip) {

    return 0;
}