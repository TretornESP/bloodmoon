#ifndef _SLINE_H
#define _SLINE_H
#include <stdint.h>
#include "scheduler.h"
#include "../debugger/debug.h"
#include "pit.h"

#define SLEEP_TIMELY_WAIT 0
#define SLEEP_AWAITING_IO 1
#define SLEEP_WAITING_FOR_BUFFER 2
#define SLEEP_WAITING_FOR_INODE 3
#define SLEEP_WAITING_FOR_TTY 4

//First calls lock_scheduler() then calls _tsleep, then unlock_scheduler(), then yield()
#define TSLEEP(ticks) \
    _tsleep(ticks); \
    yield();

#define SSLEEP(seconds) \
    _ssleep(seconds); \
    yield();

#define MSLEEP(milliseconds) \
    _msleep(milliseconds); \
    yield();

#define KSLEEP(addr) \
    _ksleep(addr); \
    yield();

#define KWAKEUP(addr) \
    _kwakeup(addr);
    
void init_sline();

void _tsleep(uint64_t ticks);
void _ssleep(uint64_t seconds);
void _msleep(uint64_t milliseconds);
void _ksleep(void* address);
void _kwakeup(void* address);

#endif