#include "dbgprinter.h"
#include "../bootservices/bootservices.h"
#include "string.h"
#define BUFFERSIZE 128

char boot_conversor_buffer[BUFFERSIZE] = {0};

void dbg_print(const char * str) {
    void (*writer)(const char*, uint64_t) = get_terminal_writer();
    writer(str, strlen(str));
}

void panic(const char * str) {
    dbg_print("\nKERNEL PANIC!\n");
    dbg_print(str);
    dbg_print("\n");
    while(1);
}

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char* itoa(int64_t value, int base) {
    // check that the base if valid
    for (int i = 0; i < BUFFERSIZE; i++) {
        boot_conversor_buffer[i] = 0;
    }
    if (base < 2 || base > 36) { *boot_conversor_buffer = '\0'; return boot_conversor_buffer; }

    char* ptr = boot_conversor_buffer, *ptr1 = boot_conversor_buffer, tmp_char;
    int64_t tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return boot_conversor_buffer;
}