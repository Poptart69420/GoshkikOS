#ifndef SPINLOCK_H_
#define SPINLOCK_H_

#include <stdbool.h>

typedef volatile bool lock_t;

bool spinlock_acquire(lock_t spin);
void spinlock_acquire_pause(lock_t spin);
void spinlock_release(lock_t spin);

#endif // SPINLOCK_H_
