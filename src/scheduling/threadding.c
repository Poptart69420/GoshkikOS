#include <scheduling/threadding.h>

thread_t *thread_table[MAX_THREADS];
thread_t *current_thread = NULL;
thread_t *ready_queue = NULL;
uint32_t next_tid = 1;
uint32_t thread_count = 0;

void init_threadding(void)
{
  kprintf("Threadding...");

  for (int i = 0; i < MAX_THREADS; ++i)
  {
    thread_table[i] = NULL;
  }

  thread_t *kernel_thread = (thread_t *)kmalloc(sizeof(thread_t));
  kernel_thread->tid = 0;
  kernel_thread->state = THREAD_RUNNING;
  kernel_thread->privilege = THREAD_RING_0;
  kernel_thread->priority = THREAD_PRIO_IMMEDIATE;
  kernel_thread->next = NULL;

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
