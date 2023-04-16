#ifndef _TTY_INTERFACES_H
#define _TTY_INTERFACES_H
#include <stdint.h>
uint64_t tty_read(const char * device, uint8_t * buffer, uint32_t size);
uint64_t tty_read_echo(const char * device, uint8_t * buffer, uint32_t size);
uint64_t tty_write(const char * device, uint8_t * buffer, uint32_t size);
uint64_t tty_write_now(const char * device, const char * buffer, uint32_t size);
uint64_t tty_ioctl(const char * device, uint32_t op, void * buffer);
#endif