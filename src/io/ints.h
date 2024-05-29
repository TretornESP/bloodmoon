#ifndef _INTS_H
#define _INTS_H

#include "../sched/concurrency.h"
#include "../util/dbgprinter.h"

atomic_int_t ints_enabled = 0;
atomic_int_t ints_activated = 0;

//This justs tells us if we can enable interrupts or not
void activate_interrupts();
void deactivate_interrupts();
int interrupts_activated();

//Only activate interrupts if they are ready
void sti();
void cli();
int interrupts_enabled();
#endif