#include "fb_dd.h"
#include "../../dev/fb/framebuffer.h"

uint64_t framebuffer_dd_read(uint64_t port, uint64_t size, uint64_t skip, uint8_t* buffer) {
    struct framebuffer *fb = get_framebuffer(port);
    if (fb == 0) {
        return 0;
    }
    uint32_t x = skip % fb->width;
    uint32_t y = skip / fb->width;

    uint32_t *pixels = (uint32_t*)buffer;
    for (uint64_t i = 0; i < size; i++) {
        pixels[i] = get_pixel(port, x, y);
        x++;
        if (x >= fb->width) {
            x = 0;
            y++;
        }
    }

    return size;
}

uint64_t framebuffer_dd_write(uint64_t port, uint64_t size, uint64_t skip, uint8_t* buffer) {
    struct framebuffer *fb = get_framebuffer(port);
    if (fb == 0) {
        return 0;
    }
    uint32_t x = skip % fb->width;
    uint32_t y = skip / fb->width;

    uint32_t *pixels = (uint32_t*)buffer;
    for (uint64_t i = 0; i < size; i++) {
        draw_pixel(port, x, y, pixels[i]);
        x++;
        if (x >= fb->width) {
            x = 0;
            y++;
        }
    }

    return size;
}

uint64_t framebuffer_dd_ioctl(uint64_t port, uint32_t op, void* data) {
    (void)port;
    (void)op;
    (void)data;

    return 0;
}

struct file_operations framebuffer_fops = {
   .read = framebuffer_dd_read,
   .write = framebuffer_dd_write,
   .ioctl = framebuffer_dd_ioctl
};

void init_fb_dd() {
    register_block(DEVICE_FB, FB_DD_NAME, &framebuffer_fops);
}