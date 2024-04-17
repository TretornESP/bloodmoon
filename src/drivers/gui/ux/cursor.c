#include "../gui.h"
#include "cursor.h"

uint8_t MousePointer[] = {
    0b11111111, 0b11000000,
    0b11111111, 0b10000000,
    0b11111110, 0b00000000,
    0b11111100, 0b00000000,
    0b11111000, 0b00000000,
    0b11110000, 0b00000000,
    0b11100000, 0b00000000,
    0b11000000, 0b00000000,
    0b10000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
};

uint32_t MouseCursorBuffer[16 * 16] = {0};
uint32_t MouseCursorBufferAfter[16 * 16] = {0};
uint8_t MouseDrawn = 0;

void clear_mouse_cursor(uint8_t* mouseCursor, uint16_t posx, uint16_t posy) {
    if (!MouseDrawn) return;

    int xMax = 16;
    int yMax = 16;
    int differenceX = get_gui_width() - posx;
    int differenceY = get_gui_height() - posy;

    if (differenceX < 16) xMax = differenceX;
    if (differenceY < 16) yMax = differenceY;

    for (int y = 0; y < yMax; y++) {
        for (int x = 0; x < xMax; x++) {
            int bit = y * 16 + x;
            int byte = bit / 8; 
            if ((mouseCursor[byte] & (0b10000000 >> (x % 8)))) {
                if (gui_get_pixel(posx + x, posy + y) == MouseCursorBufferAfter[x + y * 16]) {
                    gui_draw_pixel(posx + x, posy + y, MouseCursorBuffer[x + y * 16]);
                }
            }
        }
    }
}

void draw_mouse_cursor(uint8_t* mouseCursor, uint16_t posx, uint16_t posy, uint32_t colour) {
    int xMax = 16;
    int yMax = 16;
    int differenceX = get_gui_width() - posx;
    int differenceY = get_gui_height() - posy;

    if (differenceX < 16) xMax = differenceX;
    if (differenceY < 16) yMax = differenceY;

    for (int y = 0; y < yMax; y++) {
        for (int x = 0; x < xMax; x++) {
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((mouseCursor[byte] & (0b10000000 >> (x % 8)))) {
                MouseCursorBuffer[x + y * 16] = gui_get_pixel(posx + x, posy + y);
                gui_draw_pixel(posx + x, posy + y, colour);
                MouseCursorBufferAfter[x + y * 16] = gui_get_pixel(posx + x, posy + y);
            }
        }
    }

    MouseDrawn = 1;
}

void print_mouse_cursor(uint16_t old_x, uint16_t old_y, uint16_t x, uint16_t y) {
    clear_mouse_cursor(MousePointer, old_x, old_y);
    draw_mouse_cursor(MousePointer, x, y, 0xffffffff);
}