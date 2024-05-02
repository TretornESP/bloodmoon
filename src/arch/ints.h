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
#define INTS_DEBUG_RED_NON(x)
#define INTS_DEBUG_RED(x)
#else
#define INTS_DEBUG_RED_NON(x) RED_NON(x)
#define INTS_DEBUG_RED(x) RED(x)
#endif

#define CLI() { \
    __asm__ volatile("cli"); \
    atomic_increment(&interrupts_disabled); \
    if_status = INTS_DISABLED; \
}
//    INTS_DEBUG_RED_NON(CLI_STATUS_STRINGS[if_status]);
//    INTS_DEBUG_RED(itoa(interrupts_disabled, 10));
//}

#define FAKE_CLI() { \
    atomic_decrement(&interrupts_disabled); \
    if_status = INTS_DISABLED; \
}
//    INTS_DEBUG_RED_NON(FAKE_CLI_STATUS_STRINGS[if_status])
//    INTS_DEBUG_RED(itoa(interrupts_disabled, 10));
//}

#define STI() { \
    atomic_decrement(&interrupts_disabled); \
    if (interrupts_disabled > 0) {INTS_DEBUG_RED_NON("CANT ENABLE INTS "); INTS_DEBUG_RED(itoa(interrupts_disabled, 10));} else { \
    if (interrupts_disabled < 0) atomic_set_to_zero(&interrupts_disabled); \
    __asm__ volatile("sti"); \
    if_status = INTS_ENABLED; } \
}
//    INTS_DEBUG_RED_NON(STI_STATUS_STRINGS[if_status])
//    INTS_DEBUG_RED(itoa(interrupts_disabled, 10)); }
//}

#define FAKE_STI() { \
    atomic_decrement(&interrupts_disabled); \
    if (interrupts_disabled > 0) {INTS_DEBUG_RED_NON("[FAKE] CANT ENABLE INTS "); INTS_DEBUG_RED(itoa(interrupts_disabled, 10));} else { \
    if (interrupts_disabled < 0) atomic_set_to_zero(&interrupts_disabled); \
    if_status = INTS_ENABLED; } \
}
//    INTS_DEBUG_RED_NON(FAKE_STI_STATUS_STRINGS[if_status])
//    INTS_DEBUG_RED(itoa(interrupts_disabled, 10));
//}
    
void get_if_status(volatile uint8_t * status);
#endif
