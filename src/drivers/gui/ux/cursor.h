#ifndef _CURSOR_H
#define _CURSOR_H

#define CURSOR_FONT_WITH 8
#define CURSOR_FONT_HEIGHT 16

#include <stdint.h>
#include "psf1.h"

struct cursor {
    uint16_t x;
    uint16_t y;

    uint16_t x_min;
    uint16_t x_max;
    uint16_t y_min;
    uint16_t y_max;

    struct psf1_header * font;
    uint16_t font_width;
    uint16_t font_height;

    uint32_t color;
};

void print_mouse_cursor(uint16_t old_x, uint16_t old_y, uint16_t x, uint16_t y);

#endif