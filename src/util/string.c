#include "string.h"

size_t strlen(const char *str) {
    size_t len = 0;
    while (*str++ && len < STR_MAX_SIZE) {
        len++;
    }
    return len;
}