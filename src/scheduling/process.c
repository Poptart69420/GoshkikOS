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

static void process_cleanup(process_t *child, int index, pid_t pid, int *exit_code, int code)
{
  if (pid != -1) // If anything but -1 process ID is passed (index cannot be used for -1)
  {
    lock_proc_table();           // Lock the process table mutex
    process_table[index] = NULL; // Set the table slot of index to NULL
    unlock_proc_table();         // Unlock the process table mutex
  }
  else // -1 process ID is passed
  {
    lock_proc_table();                  // Lock process table mutex
    for (int i = 0; i < MAX_PROCS; ++i) // Have to reiterate through the table
      if (process_table[i] == child)    // If the table slot contains the process
        process_table[i] = NULL;        // Set it to NULL
    unlock_proc_table();                // Unlock process table mutex
  }

  if (child->address_space)              // If child has address space stored
    kfree((void *)child->address_space); // Free the address space

  if (exit_code)       // If exit code
    *exit_code = code; // Pass the code to the pointer passed by the caller

  kfree(child); // Free the child
}

void wake_waiters(process_t *p)
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
  thread_t *self = kernel->current_thread; // Current thread
  if (!self)                               // If no current thread (shouldn't ever happen)
    hcf();                                 // Halt catch fire

  process_t *p = process_from_pid(self->o_pid); // Get current process
  if (!p)                                       // If no process
    hcf();                                      // Halt and catch fire (does it need to be this drastic?)

  acquire_mutex(&p->lock); // Acquire the per process mutex
  p->exit_code = code;     // Store the exit code passed by the caller
  p->state = PROC_ZOMBIE;  // Mark process state as zombie
  release_mutex(&p->lock); // Release per process mutex

  wake_waiters(p); // Wake up the waiter processes

  thread_exit(); // Exit the thread

  __builtin_unreachable(); // Shouldn't reach
}

int process_terminate(pid_t pid)
{
  if (pid == 0)    // Kernel process, cannot terminate
    return -EPERM; // Return no perm errno

  process_t *p = process_from_pid(pid); // Get the process from the process ID
  if (!p)                               // If no process
    return -ESRCH;                      // Return process not found (search error) errno

  acquire_mutex(&p->lock); // Acquire the per process mutex
  p->state = PROC_ZOMBIE;  // Mark process as a zombie (to be cleaned up later)

  if (p->main_thread)                      // If process has a main thread
    thread_terminate(p->main_thread->tid); // Terminate the main thread

  release_mutex(&p->lock); // Release the per process mutex
  return 0;                // Return 0 (yayyyy)
}

int process_wait(pid_t pid, int *exit_code)
{
  thread_t *cur = kernel->current_thread; // Current running thread
  if (!cur)                               // If no running thread (should never happen)
    return -EINVAL;                       // Return invalid argument errno

  pid_t cur_pid = cur->o_pid; // Owner process of the current running thread
  process_t *child = NULL;    // Initalize child process pointer

  lock_proc_table(); // Lock the process table mutex

  int index;

  for (int i = 0; i < MAX_PROCS; ++i) // Iterate through every slot in the process table
  {
    process_t *p = process_table[i]; // Process p = i
    if (!p)                          // If no process
      continue;                      // Skip

    if (p->ppid != cur_pid) // If processes parent process ID doesn't match current running threads process ID
      continue;             // Skip

    if (pid != -1 && p->pid != pid) // If specificed child PID doesn't match this processes PID (-1 means any child)
      continue;                     // Skip

    child = p; // If matching child is found, store it in the child process pointer
    index = i; // Store the index for later use
    break;     // Break
  }

  unlock_proc_table(); // Unlock process table mutex

  if (!child)       // If no child found
    return -ECHILD; // Return no child errno

  acquire_mutex(&child->lock);                                    // Acquire child per process mutex
  if (child->state == PROC_ZOMBIE || child->state == PROC_EXITED) // If child state is zombie or exited
  {
    int code = child->exit_code; // Get the child exit code
    child->state = PROC_EXITED;  // Mark state as exited
    release_mutex(&child->lock); // Release the child per process mutex

    process_cleanup(child, index, pid, exit_code, code); // Call the process cleanup function
    return 0;                                            // Return 0 (yay!)
  }

  cur->next = NULL;                            // Inizalize next as NULL
  if (!child->wait_head)                       // If queue is empty
    child->wait_head = child->wait_tail = cur; // Set both the head and tail to the current
  else                                         // If queue is not empty
  {
    child->wait_tail->next = cur; // Link to tail
    child->wait_tail = cur;       // Add to tail
  }

  thread_block(cur->tid); // Block the current thread

  acquire_mutex(&child->lock); // When thread resumes acquire the child per process mutex
  int code = child->exit_code; // Copy the exit code
  child->state = PROC_EXITED;  // Mark the child process as exited
  release_mutex(&child->lock); // Release the child per process mutex

  process_cleanup(child, index, pid, exit_code, code); // Call the process cleanup function
  return 0;                                            // Return 0 (yay!)
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
