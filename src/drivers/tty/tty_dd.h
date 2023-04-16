#ifndef _TTY_DD_H
#define _TTY_DD_H

#define DEVICE_TTY 0xe

#define TTY_ADD_SUBSCRIBER 0x1
#define TTY_REMOVE_SUBSCRIBER 0x2
#define TTY_MODE_RAW 0x3
#define TTY_MODE_ECHO 0x4
#define TTY_FLUSH 0x5

void init_tty_dd();
#endif