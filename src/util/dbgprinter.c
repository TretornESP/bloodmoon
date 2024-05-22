#include "dbgprinter.h"
#include "../bootservices/bootservices.h"
#include "../sched/scheduler.h"
#include "../debugger/debug.h"
#include "../io/io.h"
#include "../drivers/ps2/keyboard.h"
#include "../devices/fadt/fadt.h"
#include "../util/printf.h"
#include "string.h"
#define BUFFERSIZE 0x1000
#define MSG_NUM 0x1000

char boot_conversor_buffer[BUFFERSIZE] = {0};

#define MAXSTR(x) (strlen(x) > BUFFERSIZE ? BUFFERSIZE : strlen(x))

void __attribute__((noinline)) dbg_print(const char * str) {
    void (*writer)(const char*, uint64_t) = get_terminal_writer();
    writer(str, MAXSTR(str));
}

void putchar(char chr) {
    void (*writer)(const char*, uint64_t) = get_terminal_writer();
    writer(&chr, 1);
}

__attribute__((noreturn)) void panic_reboot(const char * str) {
    __asm__ volatile("cli");
    dbg_print("\nKERNEL PANIC!\n");
    dbg_print(str);
    reboot();
    while(1); //Supress warning
}

__attribute__((noreturn)) void panic(const char * str) {
    __asm__ volatile("cli");
    dbg_print("\nKERNEL PANIC!\n");
    dbg_print(str);
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