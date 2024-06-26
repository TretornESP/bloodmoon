#include "concurrency.h"
#include "../arch/capabilities.h"
#include "scheduler.h"
#include "sline.h"

inline int wait(semaphore_t * lock) {
	acquire_spinlock(&(lock->mut));
	while (atomic_load_explicit(&lock->val, memory_order_relaxed) <= 0);
	atomic_decrement(&lock->val);
	release_spinlock(&(lock->mut));
	return 0;
}

inline int signal(semaphore_t * lock) {
	atomic_increment(&lock->val);
	return 0;
}

inline void acquire_spinlock(spinlock_t* lock) {
	while( atomic_flag_test_and_set_explicit(&(lock->mut), memory_order_acquire ) )
	{
		__asm__ volatile("pause");
	}
	lock->flags = getRflags();
	__asm__ volatile("cli");
}

inline void acquire_lock(lock_t* lock) {
	while( atomic_flag_test_and_set_explicit( lock, memory_order_acquire ) )
    {
        //Spin until we acquire the lock
        KSLEEP(&lock);
    }
}

inline void release_spinlock(spinlock_t* lock) {
	atomic_flag_clear_explicit(&(lock->mut), memory_order_release);
	if (lock->flags & 0x200) __asm__ volatile("sti");
}

inline void release_lock(lock_t * lock )
{
    atomic_flag_clear_explicit( lock, memory_order_release );
	KWAKEUP(&lock);
}

inline int try_acquire_lock(lock_t * lock) {
	//We will try to acquire the lock, if it is already acquired, we will return 0 atomic_compare_exchange_strong
	int expected = 0;
	int desired = 1;
	return atomic_compare_exchange_strong( lock, &expected, desired );
}

inline void init_lock(lock_t * lock) {
	*lock = 0;
}