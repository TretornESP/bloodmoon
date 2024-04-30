#ifndef _DBGPRINTER_H
#define _DBGPRINTER_H
#include "stdint.h"

//Call set_debug_msg specifying __FILE__, __LINE__, and __func__ as arguments
#define READY_TO_DIE() set_debug_data(__FILE__, __LINE__, __func__)

#define RED(x) \
	dbg_print("\x1B[31m "); \
	dbg_print(x); \
	dbg_print(" at "); \
	dbg_print(__FILE__); \
	dbg_print(itoa(__LINE__, 10)); \
	dbg_print("\x1B[0m\n"); \

void set_debug_data(const char * file, int line, const char * func);
void set_debug_msg(const char * str);
void dbg_print(const char * str);
void dbg_printd(const char* str, int64_t num);
__attribute__((noreturn)) void panic(const char * str);
__attribute__((noreturn)) void panic_reboot(const char * str);
char* itoa(int64_t, int);
int64_t atoi(const char * str);
#endif