#include "io.h"

void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void insw(uint16_t port, uint8_t* buffer, int count) {
    __asm__ volatile("rep insw" :: "c"(count), "d"(port), "D"(buffer));
}

void outsw(uint16_t port, uint8_t *buffer, int count) {
    __asm__ volatile("rep outsw " : : "c"(count), "d"(port), "S"(buffer));
}

void io_wait() {
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
}