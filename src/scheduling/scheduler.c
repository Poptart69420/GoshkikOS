#include <scheduling/schedular.h>

static int scheduler_counter = 0;
extern thread_t *current_thread;
extern thread_t *ready_queue;
extern thread_t *thread_table[MAX_THREADS];
extern uint32_t thread_count;
extern tss_t tss;

static const int stride_table[PRIORITY_LEVELS] = {
    [THREAD_PRIO_LOW] = 32,
    [THREAD_PRIO_BG] = 16,
    [THREAD_PRIO_NORMAL] = 8,
    [THREAD_PRIO_HIGH] = 4,
    [THREAD_PRIO_IMPORTANT] = 2,
    [THREAD_PRIO_IMMEDIATE] = 1};

static inline int pri_index(int p)
{
  if (p < 0)
    return 0;
  if (p >= PRIORITY_LEVELS)
    return PRIORITY_LEVELS - 1;
  return p;
}

void add_to_queue(thread_t *thread)
{
  if (!thread)
    return;

  thread->next = NULL;
  if (!ready_queue)
  {
    ready_queue = thread;
    return;
  }

  thread_t *iterate = ready_queue;
  while (iterate->next)
    iterate = iterate->next;
  iterate->next = thread;
}

void remove_from_queue(thread_t *thread)
{
  if (!ready_queue || !thread)
    return;

  if (ready_queue == thread)
  {
    ready_queue = ready_queue->next;
    return;
  }

  thread_t *prev = ready_queue;
  thread_t *curr = ready_queue->next;

  while (curr)
  {
    if (curr == thread)
    {
      prev->next = curr->next;
      return;
    }

    prev = curr;
    curr = curr->next;
  }
}

void schedule_tick(fault_frame_t *frame)
{
  if (!frame)
    return;
  ++scheduler_counter;

  // Save current thread context
  if (current_thread)
  {
    current_thread->context = *frame;

    switch (current_thread->state)
    {
    case THREAD_RUNNING:
      current_thread->state = THREAD_READY;
      add_to_queue(current_thread);
      break;
    case THREAD_BLOCKED:
    case THREAD_TERMED:
      remove_from_queue(current_thread);
      current_thread = NULL;
      break;
    default:
      current_thread->state = THREAD_READY;
      add_to_queue(current_thread);
      break;
    }
  }

  // Pick next thread
  thread_t *next = NULL;

  for (;;)
  {
    // Ready queue first
    if (ready_queue)
    {
      next = ready_queue;
      ready_queue = ready_queue->next;
      next->next = NULL;
    }

    // Fallback to table
    if (!next)
    {
      for (int i = 1; i < MAX_THREADS; i++)
      {
        if (thread_table[i] && thread_table[i]->state == THREAD_READY)
        {
          next = thread_table[i];
          next->next = NULL;
          break;
        }
      }
    }

    // Fallback to kernel thread
    if (!next)
    {
      if (thread_table[0] && thread_table[0]->state != THREAD_TERMED)
        next = thread_table[0];
    }

    if (!next)
      return;

    if (next->state == THREAD_BLOCKED || next->state == THREAD_TERMED)
    {
      remove_from_queue(next);
      next = NULL;
      continue;
    }

    int idx = pri_index((int)next->priority);
    int stride = stride_table[idx];

    if (next->cooldown > 0)
    {
      next->cooldown--;
      add_to_queue(next);
      next = NULL;
      continue;
    }

    next->cooldown = stride - 1;
    current_thread = next;
    current_thread->state = THREAD_RUNNING;
    break;
  }

  // Restore context
  if (current_thread->privilege == THREAD_RING_3)
  {
    if (!current_thread->kernel_stack)
    {
      current_thread->state = THREAD_TERMED;
      return;
    }

    tss.rsp0 = current_thread->kernel_stack + KSTACK_SIZE - 8;

    if (!current_thread->context.rsp)
    {
      current_thread->state = THREAD_TERMED;
      schedule_tick(frame);
      return;
    }

    *frame = current_thread->context;
    frame->rflags |= 0x200;
    frame->cs = USER_CODE64;
    frame->ss = USER_DATA64;
  }
  else
  {
    *frame = current_thread->context;
  }
}
