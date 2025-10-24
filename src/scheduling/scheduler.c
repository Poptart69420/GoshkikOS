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

static void push_task(process_t *process)
{
  if (running_process_head)
  {
    running_process_head->next = process;
  }
  else
  {
    running_process_tail = process;
  }

  running_process_head = process;

  process->next = NULL;
}

// TODO: Implement signal handling and sending
static void alert_parent(process_t *process)
{
  (void)process;
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

  process->context.cs = KERNEL_CODE64;
  process->context.ss = KERNEL_DATA64;
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

void yield(int add_back)
{
  if (!kernel->task_switch)
    return;

  int previous_int = have_interrupt();
  disable_interrupt();

  if (add_back)
    push_task(get_current_process());

  process_t *old = get_current_process();
  process_t *new = schedule();

  atomic_fetch_and(&old->flags, ~PROCESS_FLAG_RUN); // Old process isn't running
  atomic_fetch_or(&new->flags, PROCESS_FLAG_RUN);   // New process is running
  kernel->current_process = new;

  if (!add_back)
    atomic_fetch_or(&new->flags, PROCESS_FLAG_RUN);

  if (old->address_space != new->address_space)
    vmm_load_cr3(new->address_space);

  set_kernel_stack(KSTACK_TOP(new->kernel_stack));

  if (new != old)
    context_switch(&old->context, &get_current_process()->context);

  if (previous_int)
    enable_interrupt();
}

int block_process(void)
{
  atomic_fetch_and(&get_current_process()->flags, ~PROCESS_FLAG_INTR);

  int inturrupt = have_interrupt();
  disable_interrupt();

  if (!running_process_tail)
  {
    unblock_process(idle);
  }

  kernel->task_switch = 1;

  yield(0);

  if (inturrupt)
    enable_interrupt();

  if (atomic_load(&get_current_process()->flags) & PROCESS_FLAG_INTR)
  {
    return -EINTR;
  }

  return 0;
}
