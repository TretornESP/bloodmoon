#ifndef _INTS_H
#define _INTS_H

#include "../util/dbgprinter.h"
#include "../scheduling/concurrency.h"


#define INTS_ENABLED 0
#define INTS_DISABLED 1

extern volatile uint8_t if_status;
extern atomic_int_t interrupts_disabled;

extern const char * CLI_STATUS_STRINGS[];
extern const char * STI_STATUS_STRINGS[];
extern const char * FAKE_CLI_STATUS_STRINGS[];
extern const char * FAKE_STI_STATUS_STRINGS[];

#define DEBUG_INTS 1
#ifndef DEBUG_INTS
#define INTS_DEBUG_RED(x)
#else
#define INTS_DEBUG_RED(x) RED(x)
#endif

#define CLI() \
    atomic_increment(&interrupts_disabled); \
    __asm__ volatile("cli"); \
    INTS_DEBUG_RED(CLI_STATUS_STRINGS[if_status]) \
    if_status = INTS_DISABLED;

#define FAKE_CLI() \
    INTS_DEBUG_RED(FAKE_CLI_STATUS_STRINGS[if_status]) \

#define STI() \
    atomic_decrement(&interrupts_disabled); \
    if (interrupts_disabled < 0) {INTS_DEBUG_RED("CANT ENABLE INTS"); return;} \
    __asm__ volatile("sti"); \
    INTS_DEBUG_RED(STI_STATUS_STRINGS[if_status]) \
    if_status = INTS_ENABLED;

#define FAKE_STI() \
    INTS_DEBUG_RED(FAKE_STI_STATUS_STRINGS[if_status]) \
    
void get_if_status(volatile uint8_t * status);
#endif
