#ifndef _CONCURRENCY_H
#define _CONCURRENCY_H

#include <stdint.h>
#include <stdatomic.h>
#include "../debugger/debug.h"
#include "../util/printf.h"
#include "../util/dbgprinter.h"

typedef volatile _Atomic(int) atomic_int_t;
typedef volatile _Atomic(uint64_t) atomic_uint64_t;
typedef volatile _Atomic(uint8_t) atomic_uint8_t;
typedef volatile _Atomic(int) lock_t;
typedef volatile _Atomic(int) atomic_flag_t;
typedef volatile struct {
	volatile uint64_t flags;
	volatile lock_t mut;
} spinlock_t;
typedef volatile struct {
	volatile atomic_int_t val;
	volatile spinlock_t mut;
} semaphore_t;


void init_lock(lock_t * lock);
void acquire_lock(lock_t* lock);
void release_lock(lock_t* lock);
int try_acquire_lock(lock_t * lock);

void acquire_spinlock(spinlock_t* lock);
void release_spinlock(spinlock_t* lock);

int wait(semaphore_t * lock);
int signal(semaphore_t * lock);
#define LOCK(x) \
	acquire_lock(x);
#define UNLOCK(x) \
	release_lock(x);
#define LOCK_NS(x) \
	acquire_spinlock(x);
#define UNLOCK_NS(x) \
	release_spinlock(x);

static inline void atomic_set_to_zero(atomic_int_t * value) {
	atomic_store_explicit( value, 0, memory_order_relaxed );
}

static inline void atomic_sub_u64(atomic_uint64_t * value, uint64_t sub) {
	atomic_fetch_sub_explicit( value, sub, memory_order_relaxed );
}

static inline void atomic_increment(atomic_int_t * value) {
	atomic_fetch_add_explicit( value, 1, memory_order_relaxed );
}

static inline void atomic_decrement(atomic_int_t * value) {
	atomic_fetch_sub_explicit( value, 1, memory_order_relaxed );
}

static inline void atomic_increment_u64(atomic_uint64_t * value) {
	atomic_fetch_add_explicit( value, 1, memory_order_relaxed );
}

static inline void atomic_decrement_u64(atomic_uint64_t * value) {
	atomic_fetch_sub_explicit( value, 1, memory_order_relaxed );
}

static inline void atomic_increment_u8(atomic_uint8_t * value) {
	atomic_fetch_add_explicit( value, 1, memory_order_relaxed );
}

static inline void atomic_decrement_u8(atomic_uint8_t * value) {
	atomic_fetch_sub_explicit( value, 1, memory_order_relaxed );
}
#endif