#ifndef _INTS_H
#define _INTS_H
#include "../util/dbgprinter.h"
#define DEBUG_INTS 1

#ifndef DEBUG_INTS
#define INTS_DEBUG_RED(x)
#else
#define INTS_DEBUG_RED(x) RED(x)
#endif

#define CLI() \
    INTS_DEBUG_RED("CLI") \
    __asm__ volatile("cli");

#define FAKE_CLI() \
    INTS_DEBUG_RED("FAKE_CLI") \

#define STI() \
    INTS_DEBUG_RED("STI") \
    __asm__ volatile("sti");

#define FAKE_STI() \
    INTS_DEBUG_RED("FAKE_STI") \

#endif