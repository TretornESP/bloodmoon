#include <stdint.h>
#include "gui.h"
#include "../../dev/fb/framebuffer.h"
#include "../../util/common.h"

uint8_t fbi = 0;

void set_fb(uint8_t index) {
    fbi = index;
}

uint8_t get_fb() {
    return fbi;
}

uint32_t get(uint8_t x, uint8_t y) {
    return get_pixel(fbi, x, y);
}

void draw_buffer8(uint32_t * buffer, uint64_t x, uint64_t y, uint64_t w, uint64_t h) {
    uint8_t * b8 = (uint8_t *)buffer;
    for (uint64_t i = 0; i < w; i++) {
        for (uint64_t j = 0; j < h; j++) {
            uint32_t color = b8[j * w + i];
            draw_pixel(fbi, x + i, y + j, color);
        }
    }
}

void draw_buffer24(uint32_t * buffer, uint64_t x, uint64_t y, uint64_t w, uint64_t h) {
    uint8_t * b24 = (uint8_t *)buffer;
    for (uint64_t i = 0; i < w; i++) {
        for (uint64_t j = 0; j < h; j++) {
            uint32_t color = (b24[j * w + i * 3] << 16) | (b24[j * w + i * 3 + 1] << 8) | b24[j * w + i * 3 + 2];
            draw_pixel(fbi, x + i, y + j, color);
        }
    }
}

void draw_buffer32(uint32_t * buffer, uint64_t x, uint64_t y, uint64_t w, uint64_t h) {
    for (uint64_t i = 0; i < w; i++) {
        for (uint64_t j = 0; j < h; j++) {
            uint32_t color = buffer[j * w + i];
            draw_pixel(fbi, x + i, y + j, color);
        }
    }
}

void draw_buffer48(uint32_t * buffer, uint64_t x, uint64_t y, uint64_t w, uint64_t h) {
    uint16_t * b48 = (uint16_t *)buffer;
    for (uint64_t i = 0; i < w; i++) {
        for (uint64_t j = 0; j < h; j++) {
            uint32_t color = (b48[j * w + i] << 8) | b48[j * w + i + 1];
            draw_pixel(fbi, x + i, y + j, color);
        }
    }
}

void draw_rect(uint64_t x, uint64_t y, uint64_t w, uint64_t h, uint32_t color) {
    for (uint64_t i = 0; i < w; i++) {
        for (uint64_t j = 0; j < h; j++) {
            draw_pixel(fbi, x + i, y + j, color);
        }
    }
}

void draw_line(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, uint32_t color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    float Xinc = dx / (float) steps;
    float Yinc = dy / (float) steps;
    float X = x1;
    float Y = y1;
    for (int i = 0; i <= steps; i++) {
        draw_pixel(fbi, X, Y, color);
        X += Xinc;
        Y += Yinc;
    }
}

void clear(uint32_t color) {
    clear_screen(fbi, color);
}