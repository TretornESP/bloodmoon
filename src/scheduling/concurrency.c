#include "concurrency.h"

void acquire_lock(volatile lock_t * lock )
{
    while( atomic_flag_test_and_set_explicit( lock, memory_order_acquire ) )
    {
        /* use whatever is appropriate for your target arch here */
        __builtin_ia32_pause();
    }
}

int try_acquire_lock(volatile lock_t * lock )
{
	//We will try to acquire the lock, if it is already acquired, we will return 0 atomic_compare_exchange_strong
	int expected = 0;
	int desired = 1;
	return atomic_compare_exchange_strong( lock, &expected, desired );
}

void release_lock(volatile lock_t * lock )
{
    atomic_flag_clear_explicit( lock, memory_order_release );
}

void init_lock(volatile lock_t * lock) {
	*lock = 0;
}

void atomic_sub_u64(volatile atomic_uint64_t * value, uint64_t sub) {
	atomic_fetch_sub_explicit( value, sub, memory_order_relaxed );
}

void atomic_increment(volatile atomic_int_t * value) {
	atomic_fetch_add_explicit( value, 1, memory_order_relaxed );
}

void atomic_decrement(volatile atomic_int_t * value) {
	atomic_fetch_sub_explicit( value, 1, memory_order_relaxed );
}

void atomic_increment_u64(volatile atomic_uint64_t * value) {
	atomic_fetch_add_explicit( value, 1, memory_order_relaxed );
}

void atomic_decrement_u64(volatile atomic_uint64_t * value) {
	atomic_fetch_sub_explicit( value, 1, memory_order_relaxed );
}

void atomic_increment_u8(volatile atomic_uint8_t * value) {
	atomic_fetch_add_explicit( value, 1, memory_order_relaxed );
}

void atomic_decrement_u8(volatile atomic_uint8_t * value) {
	atomic_fetch_sub_explicit( value, 1, memory_order_relaxed );
}