#ifndef _TTY_H
#define _TTY_H

struct tty_buffer {
    void * buffer;
    int size_filled;
    int size;
    struct tty_buffer * next;
};

struct tty {
    char name[32];
    char device[32];
    int mode;
    struct tty_buffer * input;
    struct tty_buffer * output;
    void (*signal_handler)(int);
};

#endif