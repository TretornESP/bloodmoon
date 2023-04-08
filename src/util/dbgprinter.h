#ifndef _DBGPRINTER_H
#define _DBGPRINTER_H
#include "stdint.h"

void set_debug_msg(const char * str);
void dbg_print(const char * str);
void panic(const char * str);
char* itoa(int64_t, int);
int64_t atoi(const char * str);
#endif