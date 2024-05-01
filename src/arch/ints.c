#include "ints.h"

volatile uint8_t if_status = INTS_DISABLED;
atomic_int_t interrupts_disabled = 0;

const char * CLI_STATUS_STRINGS[] = {
    "[CLI] INTS WERE ENABLED",
    "[CLI] INTS WERE DISABLED"
};

const char * STI_STATUS_STRINGS[] = {
    "[STI] INTS WERE ENABLED",
    "[STI] INTS WERE DISABLED"
};

const char * FAKE_CLI_STATUS_STRINGS[] = {
    "[FAKE_CLI] INTS WERE ENABLED",
    "[FAKE_CLI] INTS WERE DISABLED"
};

const char * FAKE_STI_STATUS_STRINGS[] = {
    "[FAKE_STI] INTS WERE ENABLED",
    "[FAKE_STI] INTS WERE DISABLED"
};

void get_if_status(volatile uint8_t * status) {
    __asm__ volatile("pushf\n\t"
                     "pop %0\n\t"
                     : "=g"(*status));
}