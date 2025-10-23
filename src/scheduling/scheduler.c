#include <kernel.h>
#include <scheduling/scheduler.h>

static process_t *running_process_tail;
static process_t *running_process_head;

list_t *process_list;

process_t *sleeping_process;
process_t *idle;
process_t *init;

static void idle_task(void)
{
  for (;;)
    if (get_current_process()->next != running_process_head)
      block_process();
}

void init_kernel_task(void)
{
  kprintf("Starting kernel task...");
  process_list = create_list();
  sleeping_process = NULL;

  process_t *kernel_task = kmalloc(sizeof(process_t));
  memset(kernel_task, 0, sizeof(process_t));

  kernel_task->parent = kernel_task;
  kernel_task->pid = 0;
  kernel_task->flags = PROCESS_FLAG_PRESENT | PROCESS_FLAG_RUN;
  kernel_task->child = create_list();
  kernel_task->mem_seg = create_list();
  kernel_task->umask = 022;

  kernel_task->kernel_stack = (uintptr_t)kmalloc(KSTACK_SIZE);
  set_kernel_stack(KSTACK_TOP(kernel_task->kernel_stack));

  kernel_task->address_space = get_address_space();
  kernel->current_process = kernel_task;

  list_append(process_list, kernel_task);

  running_process_head = NULL;
  running_process_tail = NULL;

  init = get_current_process();
  kernel->created_process_count = 1;
  kernel->task_switch = true;

  kok();
}

process_t *schedule(void)
{
  process_t *picked = running_process_tail;
  running_process_tail = running_process_tail->next;
  if (!running_process_tail)
    running_process_head = NULL;

  while (sleeping_process)
  {
    if (sleeping_process->wakeup_time.tv_sec > time.tv_sec ||
        (sleeping_process->wakeup_time.tv_sec == time.tv_sec &&
         sleeping_process->wakeup_time.tv_usec > time.tv_usec))
      break;

    unblock_process(sleeping_process);
    sleeping_process = sleeping_process->snext;
  }

  return picked;
}

process_t *new_process(void)
{
  process_t *process = kmalloc(sizeof(process_t));
  memset(process, 0, sizeof(process_t));
  process->pid = atomic_fetch_add(&kernel->created_process_count, 1);

  init_mutex(&process->sig_lock);

  process->address_space = create_address_space();
  process->parent = get_current_process();
  process->flags = PROCESS_FLAG_PRESENT | PROCESS_FLAG_BLOCKED;
  process->child = create_list();
  process->mem_seg = create_list();
  process->cred.uid = get_current_process()->cred.uid;
  process->cred.euid = get_current_process()->cred.euid;
  process->cred.suid = get_current_process()->cred.suid;
  process->cred.gid = get_current_process()->cred.gid;
  process->cred.egid = get_current_process()->cred.egid;
  process->cred.sgid = get_current_process()->cred.sgid;
  process->umask = get_current_process()->umask;

  process->kernel_stack = (uintptr_t)kmalloc(KSTACK_SIZE);
  SP_REG(process->context) = KSTACK_TOP(process->kernel_stack);

  list_append(process->parent->child, process);
  list_append(process_list, process);

  return process;
}

process_t *new_kernel_task(void (*function)(uint64_t, char **), uint64_t argc, char *argv[])
{
  process_t *process = new_process();
  __asm__ volatile(
      "pushfq\n\t"
      "pop %0"
      : "=r"(process->context.rflags)
      :
      : "memory");

  process->contex.cs = KERNEL_CODE64;
  process->contex.ss = KERNEL_DATA64;
  process->context.ds = KERNEL_DATA64;
  process->context.es = KERNEL_DATA64;
  process->context.gs = KERNEL_DATA64;
  process->context.fs = KERNEL_DATA64;
  process->context.rip = (uint64_t)function;

  process->context.rdi = argc;           // Argc
  process->context.rsi = (uint64_t)argv; // Argv

  process->context.rsp = KSTACK_TOP(process->kernel_stack);

  unblock_process(process);

  return process;
}
