#include "dbgprinter.h"
#include "../bootservices/bootservices.h"
#include "../debugger/debug.h"
#include "string.h"
#define BUFFERSIZE 128

char dbgmsg[BUFFERSIZE] = {0};
char boot_conversor_buffer[BUFFERSIZE] = {0};

void dbg_print(const char * str) {
    void (*writer)(const char*, uint64_t) = get_terminal_writer();
    writer(str, strlen(str));
}

void set_debug_msg(const char * str) {
    memset(dbgmsg, 0, BUFFERSIZE);
    strncpy(dbgmsg, str, BUFFERSIZE);
    dbgmsg[BUFFERSIZE - 1] = 0;
}

void putchar(char chr) {
    void (*writer)(const char*, uint64_t) = get_terminal_writer();
    writer(&chr, 1);
}

void panic(const char * str) {
    dbg_print("\nKERNEL PANIC!\n");
    dbg_print(str);
    dbg_print("\n");
    dbg_print(dbgmsg);

    if (!dbg_is_present()) {
        dbg_print("Debugger not present\n");
        
    } else {
        dbg_print("Debugger attached to ");
        dbg_print(dbg_get_device());
        dbg_print("\n");


        if (!dbg("[KERNEL PANIC!]\n")) {
            goto fallback;
        }
        if (!dbg("[str: %s]\n", str)) {
            goto fallback;
        }
        if (!dbg("[dbgmsg: %s]\n", dbgmsg)) {
            goto fallback;
        }
        if (!dbg_flush()) {
            goto fallback;
        }
    }

    while(1);

    fallback:
    dbg_print("Debugger failed to print panic message, probably the heap isn't working fine!\n");
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

//atoi
int64_t atoi(const char * str) {
    int64_t res = 0; // Initialize result

    // Iterate through all characters of input string and
    // update result
    for (int i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';

    // return result.
    return res;
}

void dbg_printd(const char* str, int64_t num) {
    dbg_print(str);
    dbg_print(itoa(num, 16));
    dbg_print("\n");
}