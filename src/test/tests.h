#ifndef _TESTS_H
#define _TESTS_H

#include "../util/printf.h"

#define assert(x) if (!(x)) { printf("Assertion failed: %s\n", #x);}
#define assert_eq(x, y) if ((x) != (y)) { printf("Assertion failed: %s != %s\n", #x, #y);}

void allocatest();
void allocatest_bench_crash();

void pagetest();

void hashmaptest();

void heaptest();

void listest();
#endif