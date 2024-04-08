#ifndef _PS2_DD_H
#define _PS2_DD_H

#define DEVICE_MOUSE               0x10
#define DEVICE_KEYBOARD            0xf
#define MOUSE_DD_NAME "PS2 MOUSE DRIVER\0"
#define KEYBOARD_DD_NAME "PS2 KEYBOARD DRIVER\0"

struct ps2_kbd_ioctl_subscriptor {
    void* parent;
    void (*handler)(void* data, char c, int ignore);
} __attribute__((packed));

void init_mouse_dd();
void init_keyboard_dd();

#endif