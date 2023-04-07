#ifndef _TTY_H
#define _TTY_H

#include <stdint.h>

struct tty_buffer {
    void * page;
    int size_filled;
    struct tty_buffer * next;
};

struct tty {
    char name[32];
    int id;
    int mode;
    struct tty_buffer * input;
    struct tty_buffer * output;
    int signal;
    int com_port;
    void (*signal_handler)(int);
};

uint64_t tty_get_size(struct tty* device);
uint8_t tty_read(struct tty * device, uint8_t * destination_buffer, uint64_t size, uint64_t skip);
uint8_t tty_write(struct tty * device, uint8_t * source_buffer, uint64_t size, uint64_t skip);

#endif