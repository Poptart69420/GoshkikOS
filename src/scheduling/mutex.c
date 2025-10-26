#include <kernel.h>
#include <scheduling/mutex.h>

void init_mutex(mutex_t *mutex)
{
  if (!mutex)
    return;

  spinlock_init(&mutex->lock);
  atomic_store(&mutex->locked, 0);
  mutex->owner = NULL;
  mutex->waiter_head = NULL;
  mutex->waiter_tail = NULL;
  mutex->waiter_count = 0;
}

int acquire_mutex(mutex_t *mutex)
{
  while (true)
  {
    spinlock_acquire(&mutex->lock);

    if (atomic_load(&mutex->locked) == 0)
    {
      atomic_store(&mutex->locked, 1);
      mutex->owner = kernel->current_thread;
      spinlock_release(&mutex->lock);
      return 0;
    }

    kernel->current_thread->state = THREAD_BLOCKED;
    kernel->current_thread->next = NULL;

    if (mutex->waiter_tail)
    {
      mutex->waiter_tail->next = kernel->current_thread;
      mutex->waiter_tail = kernel->current_thread;
    }
    else
      mutex->waiter_head = mutex->waiter_tail = kernel->current_thread;

    mutex->waiter_count++;
    spinlock_release(&mutex->lock);
    thread_block(kernel->current_thread->tid);
  }
}

void release_mutex(mutex_t *mutex)
{
  if (!mutex)
    return;

  spinlock_acquire(&mutex->lock);

  if (mutex->owner != kernel->current_thread)
  {
    spinlock_release(&mutex->lock);
    return;
  }

  atomic_store(&mutex->locked, 0);
  mutex->owner = NULL;

  if (mutex->waiter_head)
  {
    thread_t *next = mutex->waiter_head;
    mutex->waiter_head = next->next;

    if (!mutex->waiter_head)
      mutex->waiter_tail = NULL;

    mutex->waiter_count--;

    next->state = THREAD_READY;
    add_to_ready_queue(next);
  }

  spinlock_release(&mutex->lock);
}
