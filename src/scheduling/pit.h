#ifndef _PIT_H
#define _PIT_H

#include <stdint.h>

struct pit {
    volatile uint64_t ticks_since_boot;
    uint16_t divisor;
    uint64_t base_frequency;
};

void init_pit();

void sleep(uint64_t);
void sleep_millis(uint64_t);

void tick();
uint64_t get_ticks_since_boot();
#endif