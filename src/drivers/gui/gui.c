#include <stdint.h>
#include "gui.h"
#include "ux/ux.h"
#include "../../dev/fb/framebuffer.h"
#include "../../util/dbgprinter.h"
#include "../../util/common.h"
#include "../../vfs/vfs.h"
#include "../../util/string.h"
#include "../../memory/heap.h"
#include "../../vfs/vfs_interface.h"

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

uint16_t get_gui_width() {
    return get_framebuffer(fbi)->width; 
}
uint16_t get_gui_height() {
    return get_framebuffer(fbi)->height;
}

void draw_buffer24(uint32_t * buffer, uint64_t x, uint64_t y, uint64_t w, uint64_t h) {
    uint8_t * b24 = (uint8_t *)buffer;
    for (uint64_t i = 0; i < w; i++) {
        for (uint64_t j = 0; j < h; j++) {
            uint32_t color = rgb_to_color(b24[j * w * 3 + i * 3], b24[j * w * 3 + i * 3 + 1], b24[j * w * 3 + i * 3 + 2]);
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

uint8_t draw_image(const char * path, uint64_t x, uint64_t y) {
    int fd = vfs_file_open(path, 0, 0);
    if (fd < 0) {
        return 0;
    }

    vfs_file_seek(fd, 0, 0x2); //SEEK_END
    uint64_t size = vfs_file_tell(fd);
    vfs_file_seek(fd, 0, 0x0); //SEEK_SET

    uint8_t* buf = malloc(size);
    memset(buf, 0, size);
    vfs_file_read(fd, buf, size);
    vfs_file_close(fd);

//50 36 0a 31 32 38 30 20 38 30 35 0a 32 35 35 0a
// P  6  .  1  2  8  0     8  0  5  .  2  5  5  .
    uint16_t magic = (buf[0] << 8) | buf[1];
    if (magic != 0x5036) {
        printf("Invalid magic number\n");
        free(buf);
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

    struct ppm * image = malloc(sizeof(struct ppm));
    image->magic = magic;
    image->width = width;
    image->height = height;
    image->max = atoi(max);
    image->buffer = (uint32_t*)(bufstart);
    
    //void draw_buffer(uint32_t * buffer, uint8_t x, uint8_t y, uint8_t w, uint8_t h);
    draw_buffer24(image->buffer, x, y, width, height);
}


void startx() {
    clear(0x000000);
    print_bg();
    spawn_tty();
}