#include <kernel.h>
#include <scheduling/mutex.h>
#include <scheduling/thread.h>

// Globals (defined in here)
spinlock_t schedule_lock; // used in scheduler
mutex_t thread_mutex;     // used in thread table allocations

thread_t *ready_head;
thread_t *ready_tail;

static inline void lock_schedule_spin(void)
{
  spinlock_acquire(&schedule_lock);
}
static inline void unlock_schedule_spin(void)
{
  spinlock_release(&schedule_lock);
}
static inline void lock_thread_mutex(void)
{
  acquire_mutex(&thread_mutex);
}
static inline void unlock_thread_mutex(void)
{
  release_mutex(&thread_mutex);
}

static void idle_task(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  halt();
}

thread_t *thread_create(thread_function_t function, int argc, char **argv, thread_privilege_t privilege,
                        thread_priority_t, uint32_t owner_pid)
{
  if (!function)
    return NULL;

  lock_thread_mutex();

  if (kernel->thread_count >= MAX_THREADS)
  {
    unlock_thread_mutex();
    return NULL;
  }
}

void threading_init(void)
{
  kprintf("Threading...");

  for (int i = 0; i < MAX_THREADS; ++i)
  {
    kernel->thread_table[i] = NULL;
  }

  init_mutex(&thread_mutex);
  spinlock_init(&schedule_lock);

  thread_t *kernel_thread = (thread_t *)kmalloc(sizeof(thread_t));
  memset(kernel_thread, 0, sizeof(thread_t));
  kernel_thread->tid = 0;
  kernel_thread->state = THREAD_RUNNING;
  kernel_thread->privilege = THREAD_RING_0;
  kernel_thread->priority = THREAD_PRIO_IMMEDIATE;
  kernel_thread->next = NULL;

  kernel->current_thread = kernel_thread;
  kernel->thread_table[0] = kernel_thread;
  kernel->thread_count = 1;

  thread_t *idle = thread_create(idle_task, 0, NULL, THREAD_RING_0, THREAD_PRIO_LOW, 0);
  if (!idle)
  {
    kerror("Failed to create idle thread");
    hcf();
  }

  ready_head = idle;
  ready_tail = idle;

  kok();
}
