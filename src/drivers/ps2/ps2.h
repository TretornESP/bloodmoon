#ifndef _PS2_H
#define _PS2_H

#define PS2_DEVICE_MOUSE    0x00
#define PS2_DEVICE_KEYBOARD 0x01

#define PS2_DEVICE_EVENT    0x00
#define PS2_DEVICE_ALL      0x01

#define MOUSE_X_OVERFLOW    0b01000000
#define MOUSE_Y_OVERFLOW    0b10000000
#define MOUSE_LEFT_CLICK    0b00000001
#define MOUSE_RIGHT_CLICK   0b00000010
#define MOUSE_MIDDLE_CLICK  0b00000100

#define IOCTL_MOUSE_SUBSCRIBE           0x01
#define IOCTL_MOUSE_UNSUBSCRIBE         0x02
#define IOCTL_MOUSE_SUBSCRIBE_EVENT     0x03
#define IOCTL_MOUSE_GET_STATUS          0x04

struct ps2_mouse_status {
    uint16_t x;
    uint16_t y;
    uint8_t buttons; // 0b00000LMR
    uint8_t scroll;
    uint8_t flags;
};

uint8_t read_mouse(struct ps2_mouse_status * mouse);
void init_ps2();
void ps2_subscribe(void (*callback)(void*), uint8_t device, uint8_t event);
void ps2_unsubscribe(void (*callback)(void*));

#endif