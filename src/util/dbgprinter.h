#ifndef _DBGPRINTER_H
#define _DBGPRINTER_H
#include "stdint.h"

#define RED(x) \
	dbg_print("\x1B[31m "); \
	dbg_print(x); \
	dbg_print(" at "); \
	dbg_print(__FILE__); \
	dbg_print(itoa(__LINE__, 10)); \
	dbg_print("\x1B[0m\n"); \

#define RED_NON(x) \
	dbg_print("\x1B[31m "); \
	dbg_print(x); \
	dbg_print(" at "); \
	dbg_print(__FILE__); \
	dbg_print(itoa(__LINE__, 10)); \
	dbg_print("\x1B[0m"); \

void dbg_print(const char * str);
void dbg_printd(const char* str, int64_t num);
__attribute__((noreturn)) void panic(const char * str);
__attribute__((noreturn)) void panic_reboot(const char * str);
char* itoa(int64_t, int);
int64_t atoi(const char * str);
#endif