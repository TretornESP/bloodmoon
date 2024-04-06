#ifndef _PS2_H
#define _PS2_H

#define MOUSE_X_OVERFLOW    0b01000000
#define MOUSE_Y_OVERFLOW    0b10000000
#define MOUSE_LEFT_CLICK    0b00000001
#define MOUSE_RIGHT_CLICK   0b00000010
#define MOUSE_MIDDLE_CLICK  0b00000100

struct ps2_mouse_status {
    uint16_t x;
    uint16_t y;
    uint8_t buttons; // 0b00000LMR
    uint8_t scroll;
    uint8_t flags;
};

struct ps2_mouse_status * read_mouse();
void init_ps2();

#endif