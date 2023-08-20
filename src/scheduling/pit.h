#ifndef _PIT_H
#define _PIT_H

#include <stdint.h>

struct pit {
    uint64_t boot_epoch;
    uint64_t ticks_per_second;

    uint64_t timer_ticks ;
    uint8_t timer_subticks ;

    uint64_t preemption_ticks;
    uint8_t preemption_enabled;
};

void init_pit();
void tick();

void sleep(uint64_t);
void sleep_ticks(uint64_t);

void tick();


void set_preeption_ticks(uint64_t ticks);
void enable_preemption();
void preempt_toggle();
uint8_t requires_preemption();

uint64_t get_ticks_since_boot();
uint64_t get_epoch();

#endif