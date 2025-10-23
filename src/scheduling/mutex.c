#include <scheduling/mutex.h>
#include <scheduling/scheduler.h>

void init_mutex(mutex_t *mutex)
{
  memset(mutex, 0, sizeof(mutex_t));
}

int acquire_mutex(mutex_t *mutex)
{
  disable_interrupt();

  if (atomic_exchange(&mutex->locked, 1))
  {
    spinlock_acquire(&mutex->lock);

    struct process_struct_t *current = get_current_process();
    current->snext = NULL;

    if (mutex->waiter_head)
    {
      mutex->waiter_head->snext = current;
    }
    else
    {
      mutex->waiter_tail = current;
    }

    mutex->waiter_head = current;
    mutex->waiter_count++;

    spinlock_release(&mutex->lock);

    while (atomic_load(&mutex->locked))
    {
      block_process();
    }
  }

  enable_interrupt();
  return 0;
}

int try_acquire_mutex(mutex_t *mutex)
{
  return !atomic_exchange(&mutex->locked, 1);
}

void release_mutex(mutex_t *mutex)
{
  disable_interrupt();

  spinlock_acquire(&mutex->lock);
  if (mutex->waiter_count > 0)
  {
    struct process_struct_t *process = mutex->waiter_tail;
    mutex->waiter_tail = process->snext;
    if (!mutex->waiter_tail)
      mutex->waiter_head = NULL;
    mutex->waiter_count--;
    unblock_process(process);
  }

  atomic_store(&mutex->locked, 0);
  spinlock_release(&mutex->lock);

  enable_interrupt();
}
