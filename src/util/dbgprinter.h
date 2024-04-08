#ifndef _DBGPRINTER_H
#define _DBGPRINTER_H
#include "stdint.h"

//Call set_debug_msg specifying __FILE__, __LINE__, and __func__ as arguments
#define READY_TO_DIE() set_debug_data(__FILE__, __LINE__, __func__)

void set_debug_data(const char * file, int line, const char * func);
void set_debug_msg(const char * str);
void dbg_print(const char * str);
void dbg_printd(const char* str, int64_t num);
__attribute__((noreturn)) void panic(const char * str);
__attribute__((noreturn)) void panic_reboot(const char * str);
char* itoa(int64_t, int);
int64_t atoi(const char * str);
#endif