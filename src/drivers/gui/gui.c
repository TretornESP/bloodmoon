#include <stdint.h>
#include "gui.h"
#include "ux/ux.h"
#include "ux/psf1.h"
#include "ux/cursor.h"
#include "../ps2/ps2.h"
#include "../ps2/ps2_interface.h"
#include "../../devices/fb/framebuffer.h"
#include "../../util/dbgprinter.h"
#include "../../util/common.h"
#include "../../vfs/vfs.h"
#include "../../util/string.h"
#include "../../memory/heap.h"
#include "../../sched/scheduler.h"
#include "../../sched/pit.h"
#include "../../vfs/vfs_interface.h"
#include "../tty/tty_interface.h"
#include "../tty/tty.h"

struct gui gui = {0};

uint32_t get(uint8_t x, uint8_t y) {
    if (!gui.ready) return 0;
    return get_pixel_fb(gui.fb, x, y);
}

uint16_t get_gui_width() {
    if (!gui.ready) return 0;
    return gui.width;
}
uint16_t get_gui_height() {
    if (!gui.ready) return 0;
    return gui.height;
}

void draw_buffer24(uint32_t * buffer, uint64_t x, uint64_t y, uint64_t w, uint64_t h) {
    if (buffer == 0) return;
    if (w == 0 || h == 0) return;
    if (x + w > gui.width || y + h > gui.height) return;
    if (!gui.ready) return;

    uint8_t * b24 = (uint8_t *)buffer;
    for (uint64_t i = 0; i < w; i++) {
        for (uint64_t j = 0; j < h; j++) {
            draw_pixel_fb(gui.fb, x + i, y + j, rgb_to_color(b24[j * w * 3 + i * 3], b24[j * w * 3 + i * 3 + 1], b24[j * w * 3 + i * 3 + 2]));
        }
    }
}

void draw_buffer32(uint32_t * buffer, uint64_t x, uint64_t y, uint64_t w, uint64_t h) {
    if (buffer == 0) return;
    if (w == 0 || h == 0) return;
    if (x + w > gui.width || y + h > gui.height) return;
    if (!gui.ready) return;

    for (uint64_t i = 0; i < w; i++) {
        for (uint64_t j = 0; j < h; j++) {
            uint32_t color = buffer[j * w + i];
            draw_pixel_fb(gui.fb, x + i, y + j, color);
        }
    }
}

void draw_rect(uint64_t x, uint64_t y, uint64_t w, uint64_t h, uint32_t color) {
    if (x + w > gui.width || y + h > gui.height) return;
    if (!gui.ready) return;

    for (uint64_t i = 0; i < w; i++) {
        for (uint64_t j = 0; j < h; j++) {
            draw_pixel_fb(gui.fb, x + i, y + j, color);
        }
    }
}

void draw_rect_rounded_corners(uint64_t x, uint64_t y, uint64_t w, uint64_t h, uint32_t color, uint64_t radius) {
    if (x + w > gui.width || y + h > gui.height) return;
    if (!gui.ready) return;

    for (uint64_t i = 0; i < w; i++) {
        for (uint64_t j = 0; j < h; j++) {
            if (i < radius && j < radius) {
                if (radius - i + radius - j > radius) {
                    draw_pixel_fb(gui.fb, x + i, y + j, color);
                }
            } else if (i < radius && j >= h - radius) {
                if (radius - i + radius - (h - j) > radius) {
                    draw_pixel_fb(gui.fb, x + i, y + j, color);
                }
            } else if (i >= w - radius && j < radius) {
                if (radius - (w - i) + radius - j > radius) {
                    draw_pixel_fb(gui.fb, x + i, y + j, color);
                }
            } else if (i >= w - radius && j >= h - radius) {
                if (radius - (w - i) + radius - (h - j) > radius) {
                    draw_pixel_fb(gui.fb, x + i, y + j, color);
                }
            } else if (i < radius) {
                draw_pixel_fb(gui.fb, x + i, y + j, color);
            } else if (j < radius) {
                draw_pixel_fb(gui.fb, x + i, y + j, color);
            } else if (i >= w - radius) {
                draw_pixel_fb(gui.fb, x + i, y + j, color);
            } else if (j >= h - radius) {
                draw_pixel_fb(gui.fb, x + i, y + j, color);
            } else {
                draw_pixel_fb(gui.fb, x + i, y + j, color);
            }
        }
    }
}

void gui_draw_pixel(uint64_t x, uint64_t y, uint32_t color) {
    if (!gui.ready) return;
    draw_pixel_fb(gui.fb, x, y, color);
}

uint32_t gui_get_pixel(uint64_t x, uint64_t y) {
    if (!gui.ready) return 0;
    return get_pixel_fb(gui.fb, x, y);
}

void draw_line(uint64_t x1, uint64_t y1, uint64_t x2, uint64_t y2, uint32_t color) {
    if (!gui.ready) return;

    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    int Xinc = dx / steps;
    int Yinc = dy / steps;
    int X = x1;
    int Y = y1;
    for (int i = 0; i <= steps; i++) {
        draw_pixel_fb(gui.fb, X, Y, color);
        X += Xinc;
        Y += Yinc;
    }
}

void clear(uint32_t color) {
    if (!gui.ready) return;
    clear_screen_fb(gui.fb, color);
}

uint8_t load_image(const char * path, uint64_t x, uint64_t y, struct ppm * image) {
    if (!gui.ready) return 0;
    if (path == 0) return 0;

    int fd = vfs_file_open(path, 0, 0);
    if (fd < 0) {
        return 0;
    }

    vfs_file_seek(fd, 0, 0x2); //SEEK_END
    uint64_t size = vfs_file_tell(fd);
    vfs_file_seek(fd, 0, 0x0); //SEEK_SET

    uint8_t* buf = kmalloc(size);
    memset(buf, 0, size);
    vfs_file_read(fd, buf, size);
    vfs_file_close(fd);

    uint16_t magic = (buf[0] << 8) | buf[1];
    if (magic != 0x5036) {
        printf("Invalid magic number\n");
        kfree(buf);
        return 0;
    }

    uint8_t dots = 0;
    char mag[32] = {0};
    char dims[32] = {0};
    char max[32] = {0};
    uint8_t* bufstart = 0;
    
    for (uint64_t i = 2; i < size; i++) {
        if (buf[i] == 0x0a) {
            dots++;
            if (dots == 1) {
                memcpy(mag, buf + 2, i - 2);
                mag[i - 2] = '\0';
                bufstart = buf + i + 1;
            } else if (dots == 2) {
                memcpy(dims, bufstart, i - (bufstart - buf));
                dims[i - (bufstart - buf)] = '\0';
                bufstart = buf + i + 1;
            } else if (dots == 3) {
                memcpy(max, bufstart, i - (bufstart - buf));
                max[i - (bufstart - buf)] = '\0';
                bufstart = buf + i + 1;
                break;
            }
        }
    }

    //Split dims by space
    char* token = strtok(dims, " ");
    uint16_t width = atoi(token);
    token = strtok(NULL, " ");
    uint16_t height = atoi(token);

    image->magic = magic;
    image->width = width;
    image->height = height;
    image->max = atoi(max);
    image->buffer = (uint32_t*)(bufstart);

    return 1;
}

uint8_t draw_image(const char * path, uint64_t x, uint64_t y) {
    if (!gui.ready) return 0;
    if (path == 0) return 0;

    struct ppm * image = kmalloc(sizeof(struct ppm));
    if (!load_image(path, x, y, image)) {
        kfree(image);
        return 0;
    }
    
    draw_buffer24(image->buffer, x, y, image->width, image->height);
}

void draw_circle(uint64_t x, uint64_t y, uint64_t r, uint32_t color) {
    int X = r, Y = 0;
    int P = 1 - r;
    while (X > Y) {
        draw_pixel_fb(gui.fb, X + x, Y + y, color);
        draw_pixel_fb(gui.fb, Y + x, X + y, color);
        draw_pixel_fb(gui.fb, -X + x, Y + y, color);
        draw_pixel_fb(gui.fb, -Y + x, X + y, color);
        draw_pixel_fb(gui.fb, -X + x, -Y + y, color);
        draw_pixel_fb(gui.fb, -Y + x, -X + y, color);
        draw_pixel_fb(gui.fb, X + x, -Y + y, color);
        draw_pixel_fb(gui.fb, Y + x, -X + y, color);
        if (P <= 0) {
            Y++;
            P = P + 2 * Y + 1;
        } else {
            X--;
            Y++;
            P = P + 2 * Y - 2 * X + 1;
        }
    }
}

void newLine(struct cursor * cursor) {
    if (cursor == 0) return;
    if (cursor->x_min == cursor->x_max || cursor->y_min == cursor->y_max) {
        return;
    }

    cursor->x = cursor->x_min;
    cursor->y += cursor->font_height;

    //TODO: Implement scrolling
    if (cursor->y + cursor->font_height > cursor->y_max) {
        cursor->y = cursor->y_min;
        clear(0x000000);
    }
}

void _putChar(char chr, struct cursor * cursor) {
    if (cursor == 0) return;
    if (cursor->x_min == cursor->x_max || cursor->y_min == cursor->y_max) {
        return;
    }

    char* fontPtr = (char*)((uint64_t)cursor->font + sizeof(struct psf1_header) + (chr * cursor->font_width));
    for (int y = cursor->y; y < cursor->y + cursor->font_height; y++) {
        for (int x = cursor->x; x < cursor->x + cursor->font_width; x++) {
            if ((*fontPtr & (0b10000000 >> (x - cursor->x))) > 0)
                draw_pixel_fb(gui.fb, x, y, cursor->color);
        }
        fontPtr++;
    }
}

void putChar(char chr, struct cursor * cursor) {
    if (cursor == 0) return;
    if (cursor->x_min == cursor->x_max || cursor->y_min == cursor->y_max) {
        return;
    }
    _putChar(chr, cursor);
    cursor->x += cursor->font_width;
    if (cursor->x + cursor->font_width > cursor->x_max) {
        newLine(cursor);
    }
}

void draw_text(uint64_t x, uint64_t y, const char * text, struct cursor * cursor) {
    if (cursor == 0) return;
    if (cursor->x_min == cursor->x_max || cursor->y_min == cursor->y_max) {
        return;
    }

    cursor->x = x;
    cursor->y = y;

    for (uint64_t i = 0; i < strlen(text); i++) {
        switch (text[i]) {
            case '\n':
                newLine(cursor);
                break;
            default: {
                //Check if text is printable ascii
                if (text[i] >= 0x20 && text[i] <= 0x7e) {
                    putChar(text[i], cursor);
                }                    
                break;
            }
        }
    }
}

void gui_dispatcher(struct ux_event * event) {
    if (!gui.ready) return;
    if (!gui.fb) return;

    event->processed = 0;
    while (!event->processed) {
        event_dispatch(event);
    }
}

void process_mouse_movement(struct ps2_mouse_status* status) {
    if (!gui.ready) return;
    if (!gui.fb) return;

    gui.mousex_old = gui.mousex;
    gui.mousey_old = gui.mousey;
    gui.mousex = status->x;
    gui.mousey = status->y;

}

void gui_dispatch_keypress_cb(void* data, char c, int ignore) {
    if (!gui.ready) return;
    if (!gui.fb) return;

    struct ux_event event = {
        .device = UX_EVENT_DEVICE_KEYBOARD,
        .x = 0,
        .y = 0,
        .data = c,
    };

    gui_dispatcher(&event);
}

void gui_dispatch_mouse_cb(void* data, char c, int ignore) {
    if (!gui.ready) return;
    if (!gui.fb) return;

    struct ps2_mouse_status * mouse = (struct ps2_mouse_status*)data;

    struct ux_event event = {
        .device = UX_EVENT_DEVICE_MOUSE,
        .x = mouse->x,
        .y = mouse->y,
        .data = mouse->buttons,
    };
    
    gui_dispatcher(&event);
    process_mouse_movement(mouse);
}

uint8_t gui_is_ready() {
    return gui.ready;
}

void render() {
    redraw_ux();
    print_mouse_cursor(gui.mousex_old, gui.mousey_old, gui.mousex, gui.mousey);
    flip_buffers_fb(gui.fb);
    uint8_t need_flip = 0;
    while (1) {
        need_flip = 0;
        if (requires_redraw()) {
            redraw_ux();
            need_flip = 1;
        }
        if (gui.mousex != gui.mousex_old || gui.mousey != gui.mousey_old) {
            print_mouse_cursor(gui.mousex_old, gui.mousey_old, gui.mousex, gui.mousey);
            need_flip = 1;
        }
        if (need_flip) {
            flip_buffers_fb(gui.fb);
        }
        msleep(1000 / GUI_FPS);
    }
}

void startx(struct gui_device * devices, uint8_t count) {
    if (gui.ready) return;

    for (uint8_t i = 0; i < count; i++) {
        printf("[STARTX] Processing device %s\n", devices[i].name);
        struct device * dev = device_search(devices[i].name);

        if (dev == 0) {
            printf("Device %s not found\n", devices[i].name);
            continue;
        }

        memcpy(&gui.devices[i], &devices[i], sizeof(struct gui_device));

        switch (devices[i].type) {
            case GUI_FB_DEVICE:
                struct framebuffer * fb = (struct framebuffer*)get_framebuffer(dev->internal_id);
                if (fb == 0) {
                    printf("Startx invalid framebuffer!\n");
                    return 0;
                }

                gui.fb = fb;
                gui.width = fb->width;
                gui.height = fb->height;
                break;  
            case GUI_KEYBOARD_DEVICE:
                struct ps2_kbd_ioctl_subscriptor sync_data = {
                    .parent = 0x0,
                    .handler = gui_dispatch_keypress_cb
                };
                ps2_subscribe_keyboard(dev->name, (void*)&sync_data);
                break;
            case GUI_MOUSE_DEVICE:
                ps2_subscribe_mouse(dev->name, gui_dispatch_mouse_cb);
                break;
            default:
                printf("Unknown device type %d\n", devices[i].type);
                break;
        }
    }

    if (gui.fb == 0) {
        printf("No framebuffer found\n");
        return;
    }

    printf("[STARTX] Starting GUI...\n");
    gui.ready = 1;

    clear(0x000000);
    load_font();
    preload_ux();
    add_task(create_task(render, get_current_tty(), KERNEL_TASK, 0x0));
}