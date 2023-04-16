#ifndef _TTY_H
#define _TTY_H

#include <stdint.h>
#include "../../../drivers/serial/serial.h"

#define TTY_MODE_CANONICAL 0
#define TTY_MODE_RAW 1
#define TTY_SIGNAL_READ 0
#define TTY_SIGNAL_WRITE 1

struct tty_buffer {
    void * page;
    int size_filled;
    struct tty_buffer * next;
};

struct tty {
    char name[32];
    int id;
    int mode;
    int com_port;
    struct tty_buffer * input;
    struct tty_buffer * output;
    void (*signal_handler)(int);
};

uint64_t vfs_tty_get_size(struct tty* device);
uint8_t vfs_tty_read(struct tty * device, uint8_t * destination_buffer, uint64_t size, uint64_t skip);
uint8_t vfs_tty_write(struct tty * device, uint8_t * source_buffer, uint64_t size, uint64_t skip);

#endif