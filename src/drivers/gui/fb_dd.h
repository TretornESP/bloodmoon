#ifndef _FB_DD_H
#define _FB_DD_H
#include "../../dev/devices.h"
#include <stdint.h>

#define FB_IOCTL_PUTC 0x1
#define FB_IOCTL_PRINTSTRING 0x2

#define DEVICE_FB            0xc
#define FB_DD_NAME "FRAMEBUFFER DRIVER\0"

void init_fb_dd();
#endif