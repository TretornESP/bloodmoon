#ifndef _DBGPRINTER_H
#define _DBGPRINTER_H
#include "stdint.h"

void dbg_print(const char * str);
void panic(const char * str);
char* itoa(int, int);
#endif