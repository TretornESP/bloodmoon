#ifndef _TTY_H
#define _TTY_H
#include "ld/line_discipline.h"

#include <stdint.h>

#define TTY_EVENT_OUTB 0
#define TTY_EVENT_INB 1
#define TTY_EVENT_FLUSH_INB 2
#define TTY_EVENT_FLUSH_OUTB 3

#define TTY_FLUSH_CHAR 0xd

#define _TTY_MODE_RAW 0x0
#define _TTY_MODE_ECHO 0x1

#define TTY_MODE_SERIAL 0x0
#define TTY_MODE_PTY 0x1

struct tty_subscriber {
    void (*handler)(void* ttyb, uint8_t event);
    struct tty_subscriber * next;
};

struct tty {
    char indev[32];
    char outdev[32];

    void (*pty_read)(void* ttyb, char c, int port);
    uint64_t (*pty_write)(const char * device, uint64_t size, uint64_t offset, uint8_t* buffer);

    volatile int signal;
    int mode;
    int valid;

    uint8_t echo;
    uint8_t israw;

    struct line_discipline* line_discipline;
    struct tty_subscriber * subscribers;

    char * inb;
    int inb_size;
    int inb_write;
    int inb_read;

    char * outb;
    int outb_size;
    int outb_write;
    int outb_read;
};

int tty_init(char* in, char* out, int mode, int inbs, int outbs);
void tty_destroy(struct tty* tty);
struct tty* get_tty(int id);
int is_valid_tty(struct tty* tty);

int _tty_get_size(struct tty* tty);
void _tty_add_subscriber(struct tty* tty, void (*handler)(void*, uint8_t));
void _tty_remove_subscriber(struct tty* tty, void (*handler)(void*, uint8_t));
void _tty_read(struct tty* tty, char* buffer, int size);
void _tty_write(struct tty* tty, char* buffer, int size);
void _tty_modes(struct tty* tty, int mode, int value);
void _tty_flush(struct tty* tty);
#endif