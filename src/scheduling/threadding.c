#include <scheduling/schedular.h>
#include <scheduling/threadding.h>

thread_t *thread_table[MAX_THREADS];
thread_t *current_thread = NULL;
thread_t *ready_queue = NULL;
uint32_t next_tid = 1;
uint32_t thread_count = 0;

void init_threadding(void)
{
  kprintf("Threading...");

  for (int i = 0; i < MAX_THREADS; ++i)
    thread_table[i] = NULL;

  thread_t *kernel_thread = (thread_t *)kmalloc(sizeof(thread_t));
  if (!kernel_thread)
  {
    hcf();
  }

  kernel_thread->stack_base = (uint64_t)kmalloc(TSTACK_SIZE);
  kernel_thread->kernel_stack = (uint64_t)kmalloc(TSTACK_SIZE);

  if (!kernel_thread->stack_base || !kernel_thread->kernel_stack)
  {
    if (kernel_thread->stack_base)
      kfree((void *)kernel_thread->stack_base);
    if (kernel_thread->kernel_stack)
      kfree((void *)kernel_thread->kernel_stack);
    kfree(kernel_thread);
    halt();
  }

  kernel_thread->tid = 0;
  kernel_thread->o_pid = 0;
  kernel_thread->state = THREAD_RUNNING;
  kernel_thread->privilege = THREAD_RING_0;
  kernel_thread->priority = THREAD_PRIO_IMMEDIATE;
  kernel_thread->stack_size = TSTACK_SIZE;
  kernel_thread->time_slice = 0;
  kernel_thread->time_used = 0;
  kernel_thread->cooldown = 0;
  kernel_thread->user_data = NULL;
  kernel_thread->next = NULL;
  kernel_thread->context.rsp = kernel_thread->stack_base + TSTACK_SIZE - 8;
  kernel_thread->context.rip = 0;
  kernel_thread->context.cs = KERNEL_CODE64;
  kernel_thread->context.ss = KERNEL_DATA64;
  kernel_thread->context.rflags = 0x202;

  current_thread = kernel_thread;
  thread_table[0] = kernel_thread;
  thread_count = 1;

  kok();
}

thread_t *create_thread(thread_func_t func, void *arg, thread_privilege_t privilege, thread_priority_t priority, uint32_t o_pid)
{
  if (thread_count >= MAX_THREADS)
    return NULL;

  thread_t *thread = (thread_t *)kmalloc(sizeof(thread_t));
  if (!thread)
    return NULL;

  if (privilege == THREAD_RING_3) // User
  {
    thread->stack_base = (uint64_t)kmalloc(TSTACK_SIZE);
    thread->kernel_stack = (uint64_t)kmalloc(TSTACK_SIZE);

    if (!thread->stack_base || !thread->kernel_stack)
    {
      if (thread->stack_base)
        kfree((void *)thread->stack_base);
      if (thread->kernel_stack)
        kfree((void *)thread->kernel_stack);

      kfree(thread);
      return NULL;
    }
  }
  else // Kernel
  {
    thread->stack_base = (uint64_t)kmalloc(TSTACK_SIZE);
    thread->kernel_stack = 0;

    if (!thread->stack_base)
    {
      kfree(thread);
      return NULL;
    }
  }

  thread->tid = next_tid++;
  thread->o_pid = o_pid;
  thread->state = THREAD_READY;
  thread->privilege = privilege;
  thread->priority = priority;
  thread->stack_size = TSTACK_SIZE;
  thread->next = NULL;

  thread->context.rsp = thread->stack_base + TSTACK_SIZE - 8;
  thread->context.rip = (uint64_t)func;
  thread->context.rdi = (uint64_t)arg;
  thread->context.rflags = 0x202;
  thread->context.cs = (privilege == THREAD_RING_3) ? USER_CODE64 : KERNEL_CODE64;
  thread->context.ss = (privilege == THREAD_RING_3) ? USER_DATA64 : KERNEL_DATA64;

  for (int i = 1; i < MAX_THREADS; ++i)
  {
    if (thread_table[i] == NULL)
    {
      thread_table[i] = thread;
      break;
    }
  }

  thread_count++;
  return thread;
}

int thread_execute(thread_t *thread)
{
  if (!thread || thread->state != THREAD_READY)
    return -1;

  if (ready_queue == NULL)
  {
    ready_queue = thread;
  }
  else
  {
    thread_t *last = ready_queue;
    while (last->next)
      last = last->next;
    last->next = thread;
  }

  return 0;
}

__attribute__((noreturn)) void thread_exit(void)
{
  if (!current_thread)
    halt();

  if (current_thread->tid == 0)
  {
    halt();
  }

  thread_t *exiting_thread = current_thread;
  current_thread->state = THREAD_TERMED;
  remove_from_queue(current_thread);
  thread_count--;

  clean_up(exiting_thread);
  halt();
}

int thread_terminate(uint32_t tid)
{
  if (tid == 0)
  {
    return -1;
  }

  thread_t *target_thread = NULL;

  for (int i = 1; i < MAX_THREADS; ++i)
  {
    if (thread_table[i] && thread_table[i]->tid == tid)
    {
      target_thread = thread_table[i];
      break;
    }
  }

  if (!target_thread)
    return -2;

  if (target_thread == current_thread)
  {
    current_thread->state = THREAD_TERMED;
  }

  clean_up(target_thread);
  kfree(target_thread);
  return 0;
}

uint32_t get_tid(void)
{
  if (current_thread == NULL)
    return 0;

  return current_thread->tid;
}

uint32_t get_pid(void)
{
  if (current_thread == NULL)
    return 0;

  return current_thread->o_pid;
}

thread_t *thread_from_tid(uint32_t tid)
{
  for (int i = 0; i < MAX_THREADS; ++i)
  {
    if (thread_table[i] && thread_table[i]->tid == tid)
      return thread_table[i];
  }

  return NULL;
}

bool thread_exists(uint32_t tid)
{
  return thread_from_tid(tid) != NULL;
}

thread_state_t thread_get_state(uint32_t tid)
{
  thread_t *thread = thread_from_tid(tid);
  if (!thread)
    return -1;
  return thread->state;
}

__attribute__((noreturn)) void clean_up(thread_t *thread)
{
  halt();
}
