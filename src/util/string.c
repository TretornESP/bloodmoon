#include "string.h"
#include "printf.h"

void *memchr(const void *s, int c, size_t n) {__UNDEFINED();}
void *memmove(void *dest, const void *src, size_t n) {__UNDEFINED();}
void *strcat(char *dest, const char *src) {__UNDEFINED();}
void *strncat(char *dest, const char *src, size_t n) {__UNDEFINED();}
void *strchr(const char *str, int ch) {__UNDEFINED();}
int strcoll(const char *s1, const char *s2) {__UNDEFINED();}
size_t strcspn(const char *s, const char *reject) {__UNDEFINED();}
char *strerror(int errnum) {__UNDEFINED();}
char *strpbrk(const char *s, const char *accept) {__UNDEFINED();}
char *strrchr(const char *str, int ch) {
    char *last = 0;
    while (*str != '\0') {
        if (*str == ch) {
            last = (char *)str;
        }
        str++;
    }
    if (*str == ch) {
        last = (char *)str;
    }
    return last;
}
size_t strspn(const char *s, const char *accept) {__UNDEFINED();}
char *strstr(const char *haystack, const char *needle) {__UNDEFINED();}
size_t strxfrm(char *dest, const char *src, size_t n) {__UNDEFINED();}

char* strtok(char* s, const char* delim) {
    static char* lastToken = 0;
    if (s != 0) {
        lastToken = s;
    } else if (lastToken == 0) {
        return 0;
    }

    char* tokenStart = lastToken;

    while (*lastToken != '\0') {
        const char* d = delim;
        while (*d != '\0') {
            if (*lastToken == *d) {
                *lastToken = '\0';
                lastToken++;
                if (tokenStart == lastToken - 1) {
                    tokenStart = lastToken;
                    continue;
                }
                return tokenStart;
            }
            d++;
        }
        lastToken++;
    }

    if (tokenStart == lastToken) {
        return 0;
    }

    return tokenStart;
}

uint64_t strlen(const char *str) {
    uint64_t len = 0;
    while (*str++ && len < STR_MAX_SIZE) {
        len++;
    }
    return len;
}

void *memset(void *dest, int val, uint64_t size) {
    uint8_t *d = (uint8_t *)dest;
    for (uint64_t i = 0; i < size; i++) {
        d[i] = val;
    }
    return dest;
}

char *strcpy(char *dest, const char *src) {
    uint64_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

void strncpy(char *dest, const char *src, uint64_t n) {
    uint64_t i = 0;
    if (strlen(src) < n) {
        n = strlen(src);
    }
    while (i < n && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void *memcpy(void *dest, const void *src, uint64_t size) {
    uint8_t *d = (uint8_t *)dest;
    uint8_t *s = (uint8_t *)src;
    for (uint64_t i = 0; i < size; i++) {
        d[i] = s[i];
    }
    return dest;
}

int zerocheck(const void *dest, uint64_t size) {
    uint8_t *d = (uint8_t *)dest;
    for (uint64_t i = 0; i < size; i++) {
        if (d[i] != 0) {
            return i;
        }
    }
    return -1;
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
	if (n == 0)
		return (0);
	do {
		if (*s1 != *s2++)
			return (*(const unsigned char *)s1 - *(const unsigned char *)--s2);
		if (*s1++ == 0)
			break;
	} while (--n != 0);
	return (0);
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