#include "spinlock.h"

bool spinlock_acquire(lock_t spin) {
    return !__atomic_test_and_set(&spin, __ATOMIC_ACQUIRE);
}

void spinlock_aqcure_pause(lock_t spin) {
acquire_lock:
    if (spinlock_acquire(spin)) {
        return;
    }

    for (;;) {
        if (__atomic_load_n(&spin, __ATOMIC_RELAXED) == 0) {
            goto acquire_lock;
        }
        __asm__ volatile("pause");
    }
}

void spinlock_release(lock_t spin) { __atomic_clear(&spin, __ATOMIC_RELEASE); }
