#ifndef _GUI_H
#define _GUI_H
#include <stdint.h>
void set_fb(uint8_t index);
uint8_t get_fb();
uint32_t get(uint8_t x, uint8_t y);
void draw_buffer8(uint32_t * buffer, uint64_t x, uint64_t y, uint64_t w, uint64_t h);
void draw_buffer24(uint32_t * buffer, uint64_t x, uint64_t y, uint64_t w, uint64_t h);
void draw_buffer32(uint32_t * buffer, uint64_t x, uint64_t y, uint64_t w, uint64_t h);
void draw_buffer48(uint32_t * buffer, uint64_t x, uint64_t y, uint64_t w, uint64_t h);
void draw_rect(uint64_t x, uint64_t y, uint64_t w, uint64_t h, uint32_t color);
void draw_line(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, uint32_t color);
void clear(uint32_t color);
#endif