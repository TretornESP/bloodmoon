#include "framebuffer.h"
#include "../../util/printf.h"
#include "../../bootservices/bootservices.h"
#include "../../memory/heap.h"

/*
struct bmoon_video_mode {
    uint64_t pitch;
    uint64_t width;
    uint64_t height;
    uint16_t bpp;
    uint8_t memory_model;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
};

struct bmoon_limine_framebuffer {
    void *address;
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
    uint8_t unused[7];
    uint64_t edid_size;
    void *edid;
    uint64_t mode_count;
    struct bmoon_video_mode **modes;
};

*/
struct framebuffer * framebuffer[MAX_FRAMEBUFFER_COUNT] = {0};

uint32_t rgb_to_color(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 16) | (g << 8) | b;
}

void color_to_rgb(uint32_t color, struct color *c) {
    c->r = (color >> 16) & 0xFF;
    c->g = (color >> 8) & 0xFF;
    c->b = color & 0xFF;
}

void init_framebuffer() {
    printf("### Initializing Framebuffer ###\n");
    uint64_t count = get_framebuffer_count();
    if (count > MAX_FRAMEBUFFER_COUNT) {
        count = MAX_FRAMEBUFFER_COUNT;
        printf("Framebuffer count exceeds maximum, truncating to %d\n", MAX_FRAMEBUFFER_COUNT);
    }
    struct bmoon_framebuffer **fb = get_framebuffers();
    for (uint64_t i = 0; i < count; i++) {
        framebuffer[i] = malloc(sizeof(struct framebuffer));
        framebuffer[i]->address = (uint32_t*)(uint64_t*)fb[i]->address;
        framebuffer[i]->font = 0;
        framebuffer[i]->width = fb[i]->width;
        framebuffer[i]->height = fb[i]->height;
        framebuffer[i]->pitch = fb[i]->pitch;
        framebuffer[i]->bpp = fb[i]->bpp;
        framebuffer[i]->memory_model = fb[i]->memory_model;
        framebuffer[i]->red_mask_size = fb[i]->red_mask_size;
        framebuffer[i]->red_mask_shift = fb[i]->red_mask_shift;
        framebuffer[i]->green_mask_size = fb[i]->green_mask_size;
        framebuffer[i]->green_mask_shift = fb[i]->green_mask_shift;
        framebuffer[i]->blue_mask_size = fb[i]->blue_mask_size;
        framebuffer[i]->blue_mask_shift = fb[i]->blue_mask_shift;
    }
}

/*
struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};
*/
struct framebuffer *get_framebuffer(uint8_t index) {
    if (index >= MAX_FRAMEBUFFER_COUNT) {
        return 0;
    }
    return framebuffer[index];
}

void draw_pixel(uint8_t index, uint64_t x, uint64_t y, uint32_t color) {
    struct framebuffer *fb = get_framebuffer(index);
    if (fb == 0) {
        return;
    }
    if (x >= fb->width || y >= fb->height) {
        return;
    }
    fb->address[(y * (fb->pitch / sizeof(uint32_t)) + x)] = color;
}

uint32_t get_pixel(uint8_t index, uint64_t x, uint64_t y) {
    struct framebuffer *fb = get_framebuffer(index);
    if (fb == 0) {
        return 0;
    }
    if (x >= fb->width || y >= fb->height) {
        return 0;
    }
    return fb->address[(y * (fb->pitch / sizeof(uint32_t)) + x)];
}

void clear_screen(uint8_t index, uint32_t color) {
    struct framebuffer *fb = get_framebuffer(index);
    if (fb == 0) {
        return;
    }
    for (uint64_t y = 0; y < fb->height; y++) {
        for (uint64_t x = 0; x < fb->width; x++) {
            draw_pixel(index, x, y, color);
        }
    }
}