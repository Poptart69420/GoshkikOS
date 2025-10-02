#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <arch/x86_64/asm/asm.h>
#include <stdatomic.h>

typedef struct spinlock_t {
    atomic_flag lock;
    int had_interrupt;
} spinlock_t;

#define spinlock_init(l) atomic_flag_clear(&(l)->lock)

#define spinlock_acquire(l)                                                    \
    do {                                                                       \
        (l)->had_interrupt = have_interrupt();                                 \
        disable_interrupt();                                                   \
        while (atomic_flag_test_and_set_explicit(&(l)->lock,                   \
                                                 memory_order_acquire))        \
            ;                                                                  \
    } while (0)

#define spinlock_release(l)                                                    \
    do {                                                                       \
        atomic_flag_clear_explicit(&(l)->lock, memory_order_release);          \
        if ((l)->had_interrupt)                                                \
            enable_interrupt();                                                \
    } while (0)

#endif // SPINLOCK_H
