#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#include <stdint.h>
#define ASCII_SIZE 56

#define LeftShift 0x2A
#define RightShift 0x36
#define Enter 0x1C
#define Backspace 0x0E
#define Spacebar 0x39

struct keyboard {
    char * ASCII_table;
    uint8_t left_shift_pressed;
    uint8_t right_shift_pressed;
};

void init_keyboard();
char translate(uint8_t, uint8_t);
void handle_keyboard(uint8_t);

#endif