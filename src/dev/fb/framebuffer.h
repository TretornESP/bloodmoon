#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <stdint.h>
#define MAX_FRAMEBUFFER_COUNT 4
#define FRAMEBUFFER_MAJOR 0x12

struct framebuffer {
    uint32_t *address; //Back buffer
    uint32_t *framebuffer;
    void * font;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp; // Bits per pixel
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
};

struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

#define rgb_to_color(r, g, b) ((r << 16) | (g << 8) | b)
#define color_to_rgb(color, c) c->r = (color >> 16) & 0xFF; c->g = (color >> 8) & 0xFF; c->b = color & 0xFF;

void init_framebuffer();
struct framebuffer *get_framebuffer(uint8_t index);
void draw_pixel(uint8_t index, uint64_t x, uint64_t y, uint32_t color);
uint32_t get_pixel(uint8_t index, uint64_t x, uint64_t y);
void clear_screen(uint8_t index, uint32_t color);
void flip_buffers_fb(struct framebuffer *fb);
void draw_pixel_fb(struct framebuffer *fb, uint64_t x, uint64_t y, uint32_t color);
uint32_t get_pixel_fb(struct framebuffer *fb, uint64_t x, uint64_t y);
void clear_screen_fb(struct framebuffer *fb, uint32_t color);

#endif