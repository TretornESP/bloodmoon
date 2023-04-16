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
    char device[32];
    
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

int tty_init(char* device, int mode, int inbs, int outbs);
void tty_destroy(struct tty* tty);
struct tty* get_tty(int id);

int is_valid_tty(struct tty* tty);
void tty_add_subscriber(struct tty* tty, void (*handler)(void*, uint8_t));
void tty_remove_subscriber(struct tty* tty, void (*handler)(void*, uint8_t));
void _tty_read(struct tty* tty, char* buffer, int size);
void _tty_write(struct tty* tty, char* buffer, int size);
void tty_modes(struct tty* tty, int mode, int value);
void tty_flush(struct tty* tty);
#endif