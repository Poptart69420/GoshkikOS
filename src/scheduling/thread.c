#include <kernel.h>
#include <scheduling/mutex.h>
#include <scheduling/scheduler.h>
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
  for (;;)
  {
    halt();
  }
}

// Helper functions
thread_t *thread_from_tid(uint32_t tid) // Lock mutex before calling otherwise TOCTOU (Time Of Check To Time Of Use race condition)
{
  thread_t *result = NULL;

  for (int i = 0; i < MAX_THREADS; ++i)
  {
    thread_t *t = kernel->thread_table[i];

    if (t && t->tid == tid)
    {
      result = t;
      break;
    }
  }

  return result;
}

//
// TODO: Find a way to make this function look less like a pile of flaming shit
//
thread_t *thread_create(thread_function_t function, int argc, char **argv, thread_privilege_t privilege,
                        thread_priority_t priority, uint32_t owner_pid)
{
  if (!function) // No function for thread
    return NULL;

  lock_thread_mutex(); // Lock mutex for thread list/table things

  if (kernel->thread_count >= MAX_THREADS) // Too many threads (what should the limit be? Need to read Linux documentation)
  {
    unlock_thread_mutex();
    return NULL;
  }

  thread_t *t = (thread_t *)kmalloc(sizeof(thread_t)); // Malloc needed space

  if (!t) // No thread (probably out of memory?)
  {
    unlock_thread_mutex();
    return NULL;
  }

  memset(t, 0, sizeof(thread_t)); // Zero out the thread

  t->tid = kernel->next_tid++; // Thread ID is the next thread ID, then increase next_tid by 1
  t->o_pid = owner_pid;        // Owner process ID
  t->state = THREAD_READY;     // Mark as ready
  t->privilege = privilege;    // Set privilege (ring 0 or ring 3)
  t->priority = priority;      // Set priority (for priority based scheduling)
  t->stack_size = TSTACK_SIZE; // Set to thread stack size (what should the stack size be? Need to read Linux documentation)
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

  memset(&t->context, 0, sizeof(context_t)); // Set needed context
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
    if (kernel->thread_table[i] == NULL) // Add to thead table (next available slot)
    {
      kernel->thread_table[i] = t;
      slot = i;
      kernel->thread_count++;
      break;
    }
  }

  if (slot < 0) // No slots
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

  return t; // Good
}

__attribute__((noreturn)) void thread_block(uint32_t tid)
{
  lock_thread_mutex();                // Lock mutex to avoid race condition
  thread_t *t = thread_from_tid(tid); // Get thread from thread ID
  unlock_thread_mutex();              // Unlock mutex

  if (!t || t->tid == 0) // No thread or it's the kernel thread
    hcf();               // Do not try and block a nonexisting thread or the kernel thread (maybe update so this is safer?)

  t->state = THREAD_BLOCKED; // Mark thread as blocked

  remove_from_ready_queue(t); // Call remove from ready queue function (defined in scheduler.c)

  __asm__ volatile("int 0x20"); // Call firmware interrupt imediately

  __builtin_unreachable(); // Shouldn't reach
}

int thread_unblock(uint32_t tid)
{
  lock_thread_mutex();                // Lock mutex to avoid race condition
  thread_t *t = thread_from_tid(tid); // Get thread ID
  unlock_thread_mutex();              // Unlock mutex

  if (!t)          // If no thread
    return -ESRCH; // No thread found

  if (t->state != THREAD_BLOCKED) // Thread is not blocked
    return -EINVAL;               // Not allowed

  t->state = THREAD_READY; // Mark thread as ready
  t->next = NULL;

  lock_schedule_spin(); // Lock for scheduling head/tail list things
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

  return 0; // Good job
}

__attribute__((noreturn)) void thread_exit(void)
{
  thread_t *current = kernel->current_thread;

  if (!current || current->tid == 0) // If no current thread or current thread is the kernel thread
    hcf();                           // Bad

  lock_thread_mutex();            // Lock mutex
  current->state = THREAD_TERMED; // Mark as terminated
  unlock_thread_mutex();          // Unlock mutex

  kernel->current_thread = NULL; // Scheduler needs to pick a different thread
  __asm__ volatile("int 0x20");  // Trigger scheduler right away
  __builtin_unreachable();       // Shouldn't reach here
}

int thread_terminate(uint32_t tid)
{
  if (tid == 0) // Do not terminate the kernel thread
    return -EPERM;

  lock_thread_mutex(); // Lock mutex to avoid race condition
  thread_t *t = thread_from_tid(tid);

  if (!t) // If no thread
  {
    unlock_thread_mutex(); // Unlock mutex
    return -ESRCH;         // Couldn't find
  }

  if (t == kernel->current_thread) // If the thread is the current thread
  {
    unlock_thread_mutex();   // Unlock thread mutex
    thread_exit();           // Use the thread exit function instead
    __builtin_unreachable(); // Shouldn't reach this
  }

  t->state = THREAD_TERMED; // Mark as terminated
  t->next = NULL;
  unlock_thread_mutex(); // Unlock mutex

  remove_from_ready_queue(t); // Call remove from ready queue function (defined in scheduler.c)
  return 0;
}

void clean_up(void)
{
  lock_thread_mutex(); // Lock thread mutex

  for (int i = 0; i < MAX_THREADS; ++i) // Loop through all threads
  {
    thread_t *t = kernel->thread_table[i];

    if (!t)
      continue; // If no thread, move onto the next

    if (t->state == THREAD_TERMED)
    {
      remove_from_ready_queue(t);

      if (t->privilege == THREAD_RING_3) // Userspace
      {
        if (t->stack_base)
          kfree((void *)t->stack_base); // Need to get userspace mem de-allocation implemented

        if (t->kernel_stack)
          kfree((void *)t->kernel_stack); // Keep as kernel memory de-allocation
      }
      else // Kernelspace
      {
        if (t->stack_base)
          kfree((void *)t->stack_base);
      }

      kfree(t);                       // Free thread
      kernel->thread_table[i] = NULL; // Set table number as NULL
      kernel->thread_count--;         // Decrase thread count by 1
    }
  }

  unlock_thread_mutex(); // Unlock thread mutex
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

  thread_t *kernel_thread = (thread_t *)kmalloc(sizeof(thread_t)); // Malloc needed space
  memset(kernel_thread, 0, sizeof(thread_t));                      // Set everything to 0 or NULL
  kernel_thread->tid = 0;
  kernel_thread->state = THREAD_RUNNING;
  kernel_thread->privilege = THREAD_RING_0; // Kernel
  kernel_thread->priority = THREAD_PRIO_IMMEDIATE;
  kernel_thread->next = NULL;

  kernel->current_thread = kernel_thread;
  kernel->thread_table[0] = kernel_thread;
  kernel->thread_count = 1; // Thread count = 1
  kernel->next_tid = 1;     // Next thread ID is 1

  thread_t *idle = thread_create(idle_task, 0, NULL, THREAD_RING_0, THREAD_PRIO_LOW, 0); // Create idle thread so scheduler works
  if (!idle)                                                                             // If no idle thread
  {
    kerror("Failed to create idle thread");
    hcf(); // Fuck
  }

  kok(); // OK!!!
}
