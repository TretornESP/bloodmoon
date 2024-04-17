#include "ps2_interface.h"
#include "../../dev/devices.h"

uint8_t ps2_keyboard_read(const char* device, char* buffer) {
    return device_read(device, 0, 0, (uint8_t*)buffer);
}
uint8_t ps2_mouse_read(const char* device, struct ps2_mouse_status* buffer) {
    return device_read(device, 0, 0, (uint8_t*)buffer);
}
uint8_t ps2_subscribe_keyboard(const char* device, void (*handler)(void* data, char c, int ignore)) {
    return device_ioctl(device, IOCTL_KEYBOARD_SUBSCRIBE, handler);
}
uint8_t ps2_subscribe_mouse(const char* device, void (*handler)(void* data, char c, int ignore)) {
    return device_ioctl(device, IOCTL_MOUSE_SUBSCRIBE, handler);
}
uint8_t ps2_subscribe_mouse_event(const char* device, void (*handler)(void* data, char c, int ignore)) {
    return device_ioctl(device, IOCTL_MOUSE_SUBSCRIBE_EVENT, handler);
}
uint8_t ps2_unsubscribe_keyboard(const char* device, void (*handler)(void* data, char c, int ignore)) {
    return device_ioctl(device, IOCTL_KEYBOARD_UNSUBSCRIBE, handler);
}
uint8_t ps2_unsubscribe_mouse(const char* device, void (*handler)(void* data, char c, int ignore)) {
    return device_ioctl(device, IOCTL_MOUSE_UNSUBSCRIBE, handler);
}