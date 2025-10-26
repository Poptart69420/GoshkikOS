#include <kernel.h>
#include <scheduling/mutex.h>
#include <scheduling/thread.h>

// Globals (defined in here)
spinlock_t schedule_lock; // Used in scheduler
mutex_t thread_mutex;     // Used in thread table allocations

thread_t *ready_head;
thread_t *ready_tail;

// Spinlock/Mutex wrappers

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

// Idle task just halt (unsure if it needs to do anything else yet)

static void idle_task(int argc, char **argv)
{
  (void)argc;
  (void)argv;
  kprintf("Idle...\n");
  halt();
}

// Helper functions
thread_t *thread_from_tid(uint32_t tid)
{
  thread_t *result = NULL;

  lock_thread_mutex();

  for (int i = 0; i < MAX_THREADS; ++i)
  {
    thread_t *t = kernel->thread_table[i];

    if (t && t->tid == tid)
    {
      result = t;
      break;
    }
  }

  unlock_thread_mutex();

  return result;
}

thread_t *thread_create(thread_function_t function, int argc, char **argv, thread_privilege_t privilege,
                        thread_priority_t priority, uint32_t owner_pid)
{
  if (!function)
    return NULL;

  lock_thread_mutex();

  if (kernel->thread_count >= MAX_THREADS)
  {
    unlock_thread_mutex();
    return NULL;
  }

  thread_t *t = (thread_t *)kmalloc(sizeof(thread_t));

  if (!t)
  {
    unlock_thread_mutex();
    return NULL;
  }

  memset(t, 0, sizeof(thread_t));

  t->tid = kernel->next_tid++;
  t->o_pid = owner_pid;
  t->state = THREAD_READY;
  t->privilege = privilege;
  t->priority = priority;
  t->stack_size = TSTACK_SIZE;
  t->next = NULL;

  if (privilege == THREAD_RING_3) // Userspace
  {
    t->stack_base = (uint64_t)kmalloc(TSTACK_SIZE);   // Need to get userspace mem allocation implemented
    t->kernel_stack = (uint64_t)kmalloc(KSTACK_SIZE); // Keep kernel mem allocation

    if (!t->stack_base || !t->kernel_stack)
    {
      if (t->stack_base)
        kfree((void *)t->stack_base); // Switch to userspace free

      if (t->kernel_stack)
        kfree((void *)t->kernel_stack); // Keep kernel mem deallocation

      kfree(t);
      unlock_thread_mutex();
      return NULL;
    }
  }
  else // Kernelspace
  {
    t->stack_base = (uint64_t)kmalloc(TSTACK_SIZE);
    t->kernel_stack = 0;

    if (!t->stack_base)
    {
      kfree(t);
      unlock_thread_mutex();
      return NULL;
    }
  }

  memset(&t->context, 0, sizeof(fault_frame_t));
  t->context.rip = (uint64_t)function;
  t->context.rsp = t->stack_base + t->stack_size - 8;
  t->context.rdi = (uint64_t)argc;
  t->context.rsi = (uint64_t)argv;
  t->context.rflags = 0x202;

  if (privilege == THREAD_RING_3)
  {
    t->context.cs = USER_CODE64;
    t->context.ss = USER_DATA64;
  }
  else
  {
    t->context.cs = KERNEL_CODE64;
    t->context.ss = KERNEL_DATA64;
  }

  int slot = -1;
  for (int i = 1; i < MAX_THREADS; ++i)
  {
    if (kernel->thread_table[i] == NULL)
    {
      kernel->thread_table[i] = t;
      slot = i;
      kernel->thread_count++;
      break;
    }
  }

  if (slot < 0)
  {
    if (privilege == THREAD_RING_3)
    {
      kfree((void *)t->stack_base);   // Switch to userspace free
      kfree((void *)t->kernel_stack); // Keep kernelspace free
    }
    else
    {
      kfree((void *)t->stack_base);
    }

    kfree(t);
    unlock_thread_mutex();
    return NULL;
  }

  unlock_thread_mutex();

  lock_schedule_spin();
  if (!ready_head)
  {
    ready_head = ready_tail = t;
  }
  else
  {
    ready_tail->next = t;
    ready_tail = t;
  }

  unlock_schedule_spin();

  return t;
}

__attribute__((noreturn)) void thread_block(uint32_t tid)
{
  thread_t *t = thread_from_tid(tid);

  if (!t || t->tid == 0)
    hcf();

  t->state = THREAD_BLOCKED;

  lock_schedule_spin();
  thread_t *previous = NULL;
  thread_t *current = ready_head;

  while (current)
  {
    if (current == t)
    {
      if (previous)
      {
        previous->next = current->next;
      }
      else
      {
        ready_head = current->next;
      }

      if (current == ready_tail)
        ready_tail = previous;

      t->next = NULL;
      break;
    }

    previous = current;
    current = current->next;
  }

  unlock_schedule_spin();

  __asm__ volatile("int 0x20");

  __builtin_unreachable();
}

int thread_unblock(uint32_t tid)
{
  thread_t *t = thread_from_tid(tid);

  if (!t)
    return -ESRCH;

  if (t->state != THREAD_BLOCKED)
    return -EINVAL;

  t->state = THREAD_READY;
  t->next = NULL;

  lock_schedule_spin();
  if (!ready_head)
  {
    ready_head = ready_tail = t;
  }
  else
  {
    ready_tail->next = t;
    ready_tail = t;
  }

  unlock_schedule_spin();

  return 0;
}

void init_threading(void)
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
  kernel->next_tid = 1;

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
