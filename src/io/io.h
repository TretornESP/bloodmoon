#ifndef _IO_H
#define _IO_H
#include <stdint.h>

void outb(uint16_t, uint8_t);
uint8_t inb(uint16_t);
void insw(uint16_t port, uint8_t* buffer, int count);
void outsw(uint16_t port, uint8_t *buffer, int count);
void io_wait();
#endif