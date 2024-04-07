#ifndef _GUI_H
#define _GUI_H
#include <stdint.h>

struct ppm {
    uint16_t magic;
    uint16_t width;
    uint16_t height;
    uint16_t max;
    uint32_t * buffer;
};

void set_fb(uint8_t index);
uint8_t get_fb();
uint32_t get(uint8_t x, uint8_t y);
uint16_t get_gui_width();
uint16_t get_gui_height();
void draw_buffer24(uint32_t * buffer, uint64_t x, uint64_t y, uint64_t w, uint64_t h);
void draw_buffer32(uint32_t * buffer, uint64_t x, uint64_t y, uint64_t w, uint64_t h);
void draw_rect(uint64_t x, uint64_t y, uint64_t w, uint64_t h, uint32_t color);
void draw_line(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, uint32_t color);
uint8_t draw_image(const char * path, uint64_t x, uint64_t y);
void clear(uint32_t color);
void startx();
#endif