#include <kernel.h>
#include <scheduling/process.h>
#include <scheduling/thread.h>

static process_t *process_table[MAX_PROCS];
static mutex_t process_table_lock;
static pid_t next_pid;

static inline void lock_proc_table(void)
{
  acquire_mutex(&process_table_lock);
}

static inline void unlock_proc_table(void)
{
  release_mutex(&process_table_lock);
}

static int find_free_slot(void)
{
  for (int i = 0; i < MAX_PROCS; ++i)
    if (process_table[i] == NULL)
      return i;
  return -1;
}

static void wake_waiters(process_t *p)
{
  if (!p)
    return;

  acquire_mutex(&p->lock);

  thread_t *cur = p->wait_head;

  while (cur)
  {
    thread_unblock(cur->tid);
    cur = cur->next;
  }

  p->wait_head = p->wait_tail = NULL;

  release_mutex(&p->lock);
}

process_t *process_from_pid(pid_t pid)
{
  if (pid < 0)
    return NULL;

  process_t *res = NULL;

  lock_proc_table();
  for (int i = 0; i < MAX_PROCS; ++i)
  {
    if (process_table[i] && process_table[i]->pid == pid)
    {
      res = process_table[i];
      break;
    }
  }

  unlock_proc_table();
  return res;
}

int process_add_thread(process_t *p, thread_t *t)
{
  if (!p || !t)
    return -EINVAL;

  acquire_mutex(&p->lock);
  t->o_pid = p->pid;
  t->proc = p;

  if (!p->main_thread)
    p->main_thread = t;

  release_mutex(&p->lock);
  return 0;
}

int process_remove_thread(process_t *p, thread_t *t)
{
  if (!p || !t)
    return -EINVAL;

  acquire_mutex(&p->lock);

  if (p->main_thread == t)
    p->main_thread = NULL;

  release_mutex(&p->lock);

  return 0;
}

process_t *process_create(thread_function_t entry, int argc, char **argv, pid_t ppid, int is_userspace)
{
  lock_proc_table();

  int slot = find_free_slot();

  if (slot < 0)
  {
    unlock_proc_table();
    return NULL;
  }

  process_t *p = (process_t *)kmalloc(sizeof(process_t));

  if (!p)
  {
    unlock_proc_table();
    return NULL;
  }

  memset(p, 0, sizeof(process_t));

  p->ppid = ppid;
  p->state = PROC_READY;
  p->umask = 022;
  init_mutex(&p->lock);
  p->address_space = is_userspace ? create_address_space() : 0;
  p->exit_code = 0;
  p->pid = next_pid++;

  unlock_proc_table();

  for (int i = 0; i < MAX_FD; ++i)
    p->fds[i].node = NULL;

  if (entry)
  {
    thread_t *t = thread_create(entry, argc, argv, is_userspace ? THREAD_RING_3 : THREAD_RING_0, THREAD_PRIO_NORMAL, p->pid);

    if (!t)
    {
      lock_proc_table();
      process_table[slot] = NULL;
      unlock_proc_table();

      if (p->address_space)
        kfree((void *)p->address_space);
      kfree(p);
      return NULL;
    }

    p->main_thread = t;
  }

  return p;
}

void process_exit(int code)
{
  thread_t *self = kernel->current_thread;
  if (!self)
    hcf();

  process_t *p = process_from_pid(self->o_pid);
  if (!p)
    hcf();

  acquire_mutex(&p->lock);
  p->exit_code = code;
  p->state = PROC_ZOMBIE;
  release_mutex(&p->lock);

  wake_waiters(p);

  thread_exit();

  __builtin_unreachable();
}

int process_terminate(pid_t pid)
{
  if (pid == 0)
    return -EPERM;

  process_t *p = process_from_pid(pid);
  if (!p)
    return -ESRCH;

  acquire_mutex(&p->lock);
  p->state = PROC_ZOMBIE;

  if (p->main_thread)
    thread_terminate(p->main_thread->tid);

  release_mutex(&p->lock);
  return 0;
}

void init_processes(void)
{
  kprintf("Processes...");
  init_mutex(&process_table_lock);

  for (int i = 0; i < MAX_PROCS; ++i)
    process_table[i] = NULL;

  process_t *kernel_process = (process_t *)kmalloc(sizeof(process_t));
  memset(kernel_process, 0, sizeof(process_t));

  kernel_process->pid = 0;
  kernel_process->ppid = 0;
  kernel_process->state = PROC_RUNNING;
  kernel_process->address_space = 0;

  init_mutex(&kernel_process->lock);

  lock_proc_table();
  process_table[0] = kernel_process;
  unlock_proc_table();
  kok();
}
