#ifndef _STRING_H
#define _STRING_H
#define STR_MAX_SIZE 65536
#include <stdint.h>

uint64_t strlen(const char *str);
void memset(void *dest, int c, uint64_t n);
void strncpy(char *dest, const char *src, uint64_t n);
void memcpy(void *dest, const void *src, uint64_t n);
uint64_t memcmp(const void *dest, const void *src, uint64_t n);

//LE Movement functions
void store32(void* dest, uint32_t value);
void store16(void* dest, uint16_t value);
uint32_t load32(const void* src);
uint16_t load16(const void* src);
#endif