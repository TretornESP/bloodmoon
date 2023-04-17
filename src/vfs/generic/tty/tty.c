#include "tty.h"
#include "../../../memory/heap.h"
#include "../../../util/string.h"
#include "../../../util/printf.h"
#include "../../../util/dbgprinter.h"
#include "../../../drivers/tty/tty_interface.h"

struct vfs_tty* tty_register_device(const char * device, uint32_t mode, const char * mountpoint) {

    struct vfs_tty* tty = (struct vfs_tty*)calloc(1, sizeof(struct vfs_tty));
    if (!tty) return 0;

    if (!tty_validate(device)) return 0;

    
    snprintf(tty->name, 32, "%s", mountpoint);
    snprintf(tty->device, 32, "%s", device);
    printf("TTY: Mounting device %s at %s\n", device, mountpoint);

    return tty;
}

void tty_unregister_device(struct vfs_tty* tty) {
    if (!tty) return;
    free(tty);
}

uint8_t tty_search(const char* name) {
    return (uint8_t)tty_identify(name);
}

int tty_sync(struct vfs_tty* tty) {
    return tty_flush(tty->device);
}

void tty_dump_device(struct vfs_tty* tty) {
    printf("TTY: %s, dev: %s\n", tty->name, tty->device);
}

uint64_t vfs_tty_get_size(struct vfs_tty* tty) {
    return tty_get_size(tty->device);
}


uint8_t vfs_tty_read(struct vfs_tty * tty, uint8_t * destination_buffer, uint64_t size, uint64_t skip) {
    return tty_read(tty->device, destination_buffer, skip, size);
}
uint8_t vfs_tty_write(struct vfs_tty * tty, uint8_t * source_buffer, uint64_t size, uint64_t skip) {
    return tty_write(tty->device, source_buffer, skip, size);
}