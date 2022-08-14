#ifndef _DEVICES_H
#define _DEVICES_H
#include <stdint.h>
const char* get_vendor_name(uint16_t);
const char* get_device_name(uint16_t, uint16_t);
const char* get_device_class(uint8_t);
const char* get_subclass_name(uint8_t, uint8_t);
const char* get_prog_interface(uint8_t, uint8_t, uint8_t);
#endif