#include "string.h"

uint64_t strlen(const char *str) {
    uint64_t len = 0;
    while (*str++ && len < STR_MAX_SIZE) {
        len++;
    }
    return len;
}

void memset(void *dest, int val, uint64_t size) {
    uint8_t *d = (uint8_t *)dest;
    for (uint64_t i = 0; i < size; i++) {
        d[i] = val;
    }
}

void strncpy(char *dest, const char *src, uint64_t n) {
    uint64_t i = 0;
    while (i < n && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void memcpy(void *dest, const void *src, uint64_t size) {
    uint8_t *d = (uint8_t *)dest;
    uint8_t *s = (uint8_t *)src;
    for (uint64_t i = 0; i < size; i++) {
        d[i] = s[i];
    }
}

 uint64_t memcmp(const void *dest, const void *src, uint64_t size) {
    uint8_t *d = (uint8_t *)dest;
    uint8_t *s = (uint8_t *)src;
    for (uint64_t i = 0; i < size; i++) {
        if (d[i] != s[i]) {
            return 1;
        }
    }
    return 0;
 }

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, uint64_t n) {
    uint64_t i = 0;
    while (i < n && s1[i] && s2[i] && s1[i] == s2[i]) {
        i++;
    }
    return s1[i] - s2[i];
}

 
void store32(void* dest, uint32_t value) {
	uint8_t* dest_ptr = (uint8_t *)dest;
	*dest_ptr++ = (uint8_t)value;
	value >>= 8;
	*dest_ptr++ = (uint8_t)value;
	value >>= 8;
	*dest_ptr++ = (uint8_t)value;
	value >>= 8;
	*dest_ptr++ = (uint8_t)value;
}

void store16(void* dest, uint16_t value) {
	uint8_t* dest_ptr = (uint8_t *)dest;
	*dest_ptr++ = (uint8_t)value;
	value >>= 8;
	*dest_ptr++ = (uint8_t)value;
}

uint64_t load64(const void* src) {
    uint64_t value = 0;
    const uint8_t* src_ptr = (const uint8_t *)src;
    value |= *src_ptr++;
    value |= (*src_ptr++ << 8);
    value |= (*src_ptr++ << 16);
    value |= (*src_ptr++ << 24);
    value |= ((uint64_t)*src_ptr++ << 32);
    value |= ((uint64_t)*src_ptr++ << 40);
    value |= ((uint64_t)*src_ptr++ << 48);
    value |= ((uint64_t)*src_ptr++ << 56);
    return value;
}

uint64_t load48(const void* src) {
    uint64_t value = 0;
    const uint8_t* src_ptr = (const uint8_t *)src;
    value |= *src_ptr++;
    value |= (*src_ptr++ << 8);
    value |= (*src_ptr++ << 16);
    value |= (*src_ptr++ << 24);
    value |= ((uint64_t)*src_ptr++ << 32);
    value |= ((uint64_t)*src_ptr++ << 40);
    return value;
}

uint32_t load32(const void* src) {
	uint32_t value = 0;
	const uint8_t* src_ptr = (const uint8_t *)src;
	value |= *src_ptr++;
	value |= (*src_ptr++ << 8);
	value |= (*src_ptr++ << 16);
	value |= (*src_ptr++ << 24);
	return value;
}

uint16_t load16(const void* src) {
	uint16_t value = 0;
	const uint8_t* src_ptr = (const uint8_t *)src;
	value |= *src_ptr++;
	value |= (*src_ptr++ << 8);
	return value;
}