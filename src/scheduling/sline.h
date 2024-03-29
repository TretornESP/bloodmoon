#ifndef _SLINE_H
#define _SLINE_H
#include <stdint.h>
#include "scheduler.h"
#include "pit.h"

#define SLEEP_TIMELY_WAIT 0
#define SLEEP_AWAITING_IO 1
#define SLEEP_WAITING_FOR_BUFFER 2
#define SLEEP_WAITING_FOR_INODE 3
#define SLEEP_WAITING_FOR_TTY 4

//First calls lock_scheduler() then calls _tsleep, then unlock_scheduler(), then yield()
#define TSLEEP(ticks) \
    lock_scheduler(); \
    _tsleep(ticks); \
    unlock_scheduler(); \
    yield();

#define SSLEEP(seconds) \
    lock_scheduler(); \
    _ssleep(seconds); \
    unlock_scheduler(); \
    yield();

#define MSLEEP(milliseconds) \
    lock_scheduler(); \
    _msleep(milliseconds); \
    unlock_scheduler(); \
    yield();

#define KSLEEP(addr) \
    lock_scheduler(); \
    _ksleep(addr); \
    unlock_scheduler(); \
    yield();

#define KWAKEUP(addr) \
    lock_scheduler(); \
    _kwakeup(addr); \
    unlock_scheduler(); \
    yield();

void init_sline();

void _tsleep(uint64_t ticks);
void _ssleep(uint64_t seconds);
void _msleep(uint64_t milliseconds);
void _ksleep(uint8_t address);
void _kwakeup(uint8_t address);

#endif