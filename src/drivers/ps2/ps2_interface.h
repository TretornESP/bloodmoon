#ifndef _PS2_INTERFACE_H
#define _PS2_INTERFACE_H

#include <stdint.h>
#include "ps2.h"

uint8_t ps2_keyboard_read(const char* device, char* buffer);
uint8_t ps2_mouse_read(const char* device, struct ps2_mouse_status* buffer);
uint8_t ps2_subscribe_keyboard(const char* device, void (*handler)(void* data, char c, int ignore));
uint8_t ps2_subscribe_mouse(const char* device, void (*handler)(void* data, char c, int ignore));
uint8_t ps2_subscribe_mouse_event(const char* device, void (*handler)(void* data, char c, int ignore));
uint8_t ps2_unsubscribe_keyboard(const char* device, void (*handler)(void* data, char c, int ignore));
uint8_t ps2_unsubscribe_mouse(const char* device, void (*handler)(void* data, char c, int ignore));
#endif