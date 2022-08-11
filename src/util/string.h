#ifndef _STRING_H
#define _STRING_H
#define STR_MAX_SIZE 65536
#include <stdint.h>

uint64_t strlen(const char *str);
void memset(void *dest, int c, uint64_t n);
void strncpy(char *dest, const char *src, uint64_t n);
void memcpy(void *dest, const void *src, uint64_t n);
#endif