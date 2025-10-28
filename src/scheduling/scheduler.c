#include <kernel.h>
#include <scheduling/scheduler.h>

// Globals (defined in thread.c)
extern spinlock_t schedule_lock;
extern mutex_t thread_mutex;
extern thread_t *ready_head;
extern thread_t *ready_tail;

// Priority counter
int priority_counter = 0;

// Spinlock/Mutex wrappers
static inline void lock_schedule_spin(void)
{
  spinlock_acquire(&schedule_lock);
}
static inline void unlock_schedule_spin(void)
{
  spinlock_release(&schedule_lock);
}
static inline void lock_table_mutex(void)
{
  acquire_mutex(&thread_mutex);
}
static inline void unlock_table_mutex(void)
{
  release_mutex(&thread_mutex);
}

// Helpers

//
// Change return numbers based on how you want scheduling to prioritize things.
// Smaller numbers means higher priority, longer numbers are a lower priority (and get less attention)
//
static inline int priority_to_stride(thread_priority_t priority) // Maps priority enum to stride for fair CPU distribution
{
  switch (priority)
  {
  case THREAD_PRIO_IMMEDIATE:
    return 1;
  case THREAD_PRIO_IMPORTANT:
    return 2;
  case THREAD_PRIO_HIGH:
    return 4;
  case THREAD_PRIO_NORMAL:
    return 8;
  case THREAD_PRIO_BG:
    return 16;
  case THREAD_PRIO_LOW:
    return 32;
  default:
    return 8; // Default is normal (if no priority, or an invalid priority is passed)
  }
}

static void enqueue_ready_tail(thread_t *t) // Lock the schedule spinlock before calling for IRQ safety
{
  if (!t)
    return;

  t->next = NULL;

  if (!ready_head)
  {
    ready_head = ready_tail = t;
  }
  else
  {
    ready_tail->next = t;
    ready_tail = t;
  }
}

static thread_t *dequeue_ready_head(void) // Lock the schedule spinlock before calling for IRQ safety
{
  thread_t *t = ready_head;
  if (!t)
    return NULL;

  ready_head = ready_head->next;
  if (!ready_head)
    ready_tail = NULL;

  t->next = NULL;
  return t;
}

void add_to_ready_queue(thread_t *t)
{
  if (!t || t->state == THREAD_TERMED)
    return;

  lock_schedule_spin();

  thread_t *check = ready_head; // Check for duplicate threads
  while (check)
  {
    if (check == t) // Duplicate found
    {
      unlock_schedule_spin();
      return;
    }

    check = check->next;
  }

  t->next = NULL;
  t->state = THREAD_READY;
  enqueue_ready_tail(t);
  unlock_schedule_spin();
}

void remove_from_ready_queue(thread_t *t)
{
  if (!t)
    return;

  lock_schedule_spin();

  thread_t *prev = NULL;
  thread_t *cur = ready_head;

  while (cur)
  {
    if (cur == t)
    {
      if (prev)
        prev->next = cur->next;
      else
        ready_head = cur->next;

      if (cur == ready_tail)
        ready_tail = prev;

      t->next = NULL;
      break;
    }

    prev = cur;
    cur = cur->next;
  }

  unlock_schedule_spin();
}

//
// TODO: Make this less fugly
//

// Main scheduler function
void scheduler_tick(context_t *context) // Called by timer IRQ, context is passed by the IRQ
{
  if (!context)
    return;

  priority_counter++;

  thread_t *old = kernel->current_thread;

  if (old) // Save old context
    old->context = *context;

  if (old) // Handle inturrupted thread
  {
    switch (old->state)
    {
    case THREAD_RUNNING: // If running set as ready and add to queue
      old->state = THREAD_READY;
      lock_schedule_spin();
      enqueue_ready_tail(old);
      unlock_schedule_spin();
      break;
    case THREAD_BLOCKED: // If blocked remove from ready queue
      lock_schedule_spin();
      remove_from_ready_queue(old);
      unlock_schedule_spin();
      kernel->current_thread = NULL;
      break;
    case THREAD_TERMED: // If terminated remove from ready queue
      lock_schedule_spin();
      remove_from_ready_queue(old);
      unlock_schedule_spin();
      kernel->current_thread = NULL;
      break;
    default: // if default set as ready and add to queue
      old->state = THREAD_READY;
      lock_schedule_spin();
      enqueue_ready_tail(old);
      unlock_schedule_spin();
      break;
    }
  }

  for (;;)
  {
    thread_t *next = NULL;

    lock_schedule_spin();
    next = dequeue_ready_head(); // Take from ready queue
    unlock_schedule_spin();

    if (!next) // If no thread is in the ready queue
    {
      lock_table_mutex();
      for (int i = 1; i < MAX_THREADS; ++i)
      {
        thread_t *t = kernel->thread_table[i];
        if (t && t->state == THREAD_READY) // If thread table has ready threads switch to that
        {
          next = t;
          break;
        }
      }

      unlock_table_mutex();
    }

    if (!next) // If no thread in ready queue & nothing is ready in the table
    {
      thread_t *t = kernel->thread_table[0];
      if (t && t->state != THREAD_TERMED) // Next thread is the kernel thread
        next = t;
    }

    if (!next) // If no thread in ready queue & nothing is ready in the table & kernel thread it terminated
      return;  // Should never happen hopefully

    if (next->state == THREAD_BLOCKED || next->state == THREAD_TERMED)
    {
      lock_schedule_spin();
      remove_from_ready_queue(next);
      unlock_schedule_spin();
      continue; // Try again
    }

    int stride = priority_to_stride(next->priority); // Get thread stride
    if (next->cooldown > 0)                          // Handle cooldown
    {
      next->cooldown--;
      lock_schedule_spin();
      enqueue_ready_tail(next);
      unlock_schedule_spin();
      continue; // Try again
    }
    else
    {
      next->cooldown = stride - 1; // If not greater than 0 reset cooldown -1
    }

    if (next->privilege == THREAD_RING_3) // If userspace thread
    {
      if (!next->kernel_stack)
      {
        next->state = THREAD_TERMED; // Mark terminated
        continue;                    // Try again
      }
    }

    kernel->current_thread = next; // Commit selection
    next->state = THREAD_RUNNING;  // Mark selected as running

    if (next->privilege == THREAD_RING_3)              // If userspace thread
      tss.rsp0 = next->kernel_stack + KSTACK_SIZE - 8; // Update TSS

    unlock_schedule_spin();
    int is_ring_3 = (next->privilege == THREAD_RING_3); // Is userspace?
    context_switch(context, &next->context, is_ring_3); // Assembly function

    clean_up();

    return;
  }
}
