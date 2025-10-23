#ifndef MUTEX_H_
#define MUTEX_H_

#include <arch/x86_64/asm/asm.h>
#include <klibc/errno.h>
#include <klibc/kmem/string.h>
#include <scheduling/spinlock.h>
#include <stdatomic.h>
#include <stddef.h>

struct process_struct_t;

typedef struct mutex_struct
{
  spinlock_t lock;
  atomic_int locked;
  struct process_struct_t *waiter_head;
  struct process_struct_t *waiter_tail;
  size_t waiter_count;
} mutex_t;

void init_mutex(mutex_t *mutex);
int acquire_mutex(mutex_t *mutex);
int try_acquire_mutex(mutex_t *mutex);
void release_mutex(mutex_t *mutex);

#endif // MUTEX_H_
