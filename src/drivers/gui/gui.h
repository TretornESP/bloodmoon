#ifndef _GUI_H
#define _GUI_H
#include <stdint.h>
#include "ux/cursor.h"
#include "../../dev/fb/framebuffer.h"

#define MAX_GUI_DEVICES 8

#define GUI_MOUSE_DEVICE 0x0
#define GUI_TTY_DEVICE 0x1
#define GUI_KEYBOARD_DEVICE 0x2
#define GUI_FB_DEVICE 0x3
#define GUI_AUDIO_DEVICE 0x4

struct ppm {
    uint16_t magic;
    uint16_t width;
    uint16_t height;
    uint16_t max;
    uint32_t * buffer;
};

struct gui_device {
    uint8_t type;
    char name[32];
};

struct gui {
    struct framebuffer * fb;
    uint16_t mousex;
    uint16_t mousey;
    uint8_t ready;
    uint16_t width;
    uint16_t height;
    struct gui_device devices[MAX_GUI_DEVICES];
};

uint8_t gui_is_ready();

uint32_t get(uint8_t x, uint8_t y);
uint16_t get_gui_width();
uint16_t get_gui_height();
void draw_buffer24(uint32_t * buffer, uint64_t x, uint64_t y, uint64_t w, uint64_t h);
void draw_buffer32(uint32_t * buffer, uint64_t x, uint64_t y, uint64_t w, uint64_t h);
void draw_rect(uint64_t x, uint64_t y, uint64_t w, uint64_t h, uint32_t color);
void draw_rect_rounded_corners(uint64_t x, uint64_t y, uint64_t w, uint64_t h, uint32_t color, uint64_t radius);
void draw_line(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, uint32_t color);
void draw_circle(uint64_t x, uint64_t y, uint64_t r, uint32_t color);
void draw_text(uint64_t x, uint64_t y, const char * text, struct cursor * cursor);

void gui_draw_pixel(uint64_t x, uint64_t y, uint32_t color);
uint32_t gui_get_pixel(uint64_t x, uint64_t y);

uint8_t draw_image(const char * path, uint64_t x, uint64_t y);
void putChar(char chr, struct cursor * cursor);
void clear(uint32_t color);
void startx(struct gui_device * devices, uint8_t count);
#endif