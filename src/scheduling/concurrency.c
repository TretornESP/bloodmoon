#include "concurrency.h"
#include "scheduler.h"

inline void acquire_lock(volatile lock_t* lock) {
	while( atomic_flag_test_and_set_explicit( lock, memory_order_acquire ) )
    {
        //Spin until we acquire the lock
        __asm__ volatile("nop");
    }
}

inline void release_lock(volatile lock_t * lock )
{
    atomic_flag_clear_explicit( lock, memory_order_release );
}

inline int try_acquire_lock(volatile lock_t * lock) {
	//We will try to acquire the lock, if it is already acquired, we will return 0 atomic_compare_exchange_strong
	int expected = 0;
	int desired = 1;
	return atomic_compare_exchange_strong( lock, &expected, desired );
}

inline void init_lock(volatile lock_t * lock) {
	*lock = 0;
}