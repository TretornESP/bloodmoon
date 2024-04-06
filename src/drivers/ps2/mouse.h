#ifndef __MOUSE_H__
#define __MOUSE_H__
#include <stdint.h>
#include "ps2.h"

typedef struct {
	long x;
	long y;
} Point;

#define PS2LeftButton   0b00000001
#define PS2RightButton  0b00000010
#define PS2MiddleButton 0b00000100
#define PS2XSign        0b00010000
#define PS2YSign        0b00100000
#define PS2XOverflow    0b01000000
#define PS2YOverflow    0b10000000

extern uint8_t MousePointer[];

void init_mouse();
void handle_mouse(uint8_t data);
uint8_t process_mouse_packet(struct ps2_mouse_status* status);
extern Point MousePosition;
#endif