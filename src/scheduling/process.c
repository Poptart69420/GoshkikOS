#include <kernel.h>
#include <scheduling/process.h>
#include <scheduling/thread.h>

// Static variables
static process_t *process_table[MAX_PROCS]; // Process table
static mutex_t process_table_lock;          // Process table mutex
static pid_t next_pid;                      // Next process ID

// Mutex wrappers
static inline void lock_proc_table(void)
{
  acquire_mutex(&process_table_lock);
}

static inline void unlock_proc_table(void)
{
  release_mutex(&process_table_lock);
}

// Helper functions
static int find_free_slot(void)
{
  for (int i = 0; i < MAX_PROCS; ++i) // Iterate through all process table slots
    if (process_table[i] == NULL)     // If slot == NULL (empty)
      return i;                       // Return the index of the free slot
  return -1;                          // No free slots found
}

static void process_cleanup(process_t *child, int index, pid_t pid, int *exit_code, int code)
{
  if (pid != -1) // If anything but -1 process ID is passed (index cannot be used for -1)
  {
    lock_proc_table();
    process_table[index] = NULL; // Set the table slot of index to NULL
    unlock_proc_table();
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

  kfree(child);
}

// Wake waiter processes (called by child processes)
void wake_waiters(process_t *p)
{
  if (!p)
    return; // Return

  acquire_mutex(&p->lock);

  thread_t *cur = p->wait_head; // Get the head of the wait queue

  while (cur) // Iterate through all waiting threads
  {
    thread_unblock(cur->tid); // Unblock waiting thread
    cur = cur->next;          // Move to next thread in the list
  }

  p->wait_head = p->wait_tail = NULL; // Clear the wait queue

  release_mutex(&p->lock);
}

// Lookup process by process ID
process_t *process_from_pid(pid_t pid)
{
  if (pid < 0)
    return NULL; // Return NULL

  process_t *res = NULL; // Initalize result as NULL

  lock_proc_table();
  for (int i = 0; i < MAX_PROCS; ++i) // Interate through the process table list
  {
    if (process_table[i] && process_table[i]->pid == pid) // If process' process ID matches the one we're searching for
    {
      res = process_table[i]; // Store the process in the result
      break;
    }
  }

  unlock_proc_table();
  return res; // Return the found process (NULL if failure to find)
}

// Add a thread to a process
int process_add_thread(process_t *p, thread_t *t)
{
  if (!p || !t)
    return -EINVAL; // Return invalid arguments errno

  acquire_mutex(&p->lock);
  t->o_pid = p->pid; // Set thread owner process ID
  t->proc = p;       // Link the thread to the process

  if (!p->main_thread)  // If no main thread set
    p->main_thread = t; // Assign as the main thread

  release_mutex(&p->lock);
  return 0;
}

// Remove a thread from a process
int process_remove_thread(process_t *p, thread_t *t)
{
  if (!p || !t)
    return -EINVAL; // Return invalid arguments errno

  acquire_mutex(&p->lock);

  if (p->main_thread == t) // If the process' main thread is the thread we are removing
    p->main_thread = NULL; // Set the process' main thread to NULL

  release_mutex(&p->lock);

  return 0;
}

// Create a process
process_t *process_create(thread_function_t entry, int argc, char **argv, pid_t ppid, int is_userspace)
{
  lock_proc_table();

  int slot = find_free_slot(); // Find an empty slot

  if (slot < 0)
  {
    unlock_proc_table();
    return NULL; // Return NULL (no free spaces)
  }

  process_t *p = (process_t *)kmalloc(sizeof(process_t)); // Allocate the process struct
  if (!p)
  {
    unlock_proc_table();
    return NULL; // Return NULL (allocation failed, out of memory?)
  }

  memset(p, 0, sizeof(process_t)); // Zero out the process struct

  p->ppid = ppid;                                               // Set the process' parent process' process ID
  p->state = PROC_READY;                                        // Mark process state as ready
  p->umask = 022;                                               // Default umask
  init_mutex(&p->lock);                                         // Initalize the per process mutex
  p->address_space = is_userspace ? create_address_space() : 0; // Create address space if process is a userspace process
  p->exit_code = 0;                                             // Initalize exit code as 0
  p->pid = next_pid++;                                          // Assign process ID, then increase the next process ID by 1

  unlock_proc_table();

  for (int i = 0; i < MAX_FD; ++i) // Loop through the file descriptor table
    p->fds[i].node = NULL;         // Initalize as NULL

  if (entry)
  {
    thread_t *t = thread_create(entry, argc, argv, is_userspace ? THREAD_RING_3 : THREAD_RING_0, THREAD_PRIO_NORMAL, p->pid); // Create the thread (passing the entry function)

    if (!t)
    {
      lock_proc_table();
      process_table[slot] = NULL; // Set the process table slot to NULL (so we don't accidentally select it)
      unlock_proc_table();

      if (p->address_space)
        kfree((void *)p->address_space);
      kfree(p);
      return NULL; // Failed to create the thread
    }

    p->main_thread = t; // Set the main thread
  }

  return p; // Return the process
}

void process_exit(int code)
{
  thread_t *self = kernel->current_thread;
  if (!self) // Shouldn't ever happen
    hcf();   // Halt catch fire

  process_t *p = process_from_pid(self->o_pid); // Get current process
  if (!p)                                       // If no process
    hcf();                                      // Halt and catch fire (does it need to be this drastic?)

  acquire_mutex(&p->lock); // Acquire the per process mutex
  p->exit_code = code;     // Store the exit code passed by the caller
  p->state = PROC_ZOMBIE;  // Mark process state as zombie
  release_mutex(&p->lock); // Release per process mutex

  wake_waiters(p); // Wake up the waiter processes (usually the parent?)

  thread_exit();

  __builtin_unreachable(); // Shouldn't reach
}

int process_terminate(pid_t pid)
{
  if (pid == 0)    // Kernel process, cannot terminate
    return -EPERM; // Return no perm errno

  process_t *p = process_from_pid(pid);
  if (!p)
    return -ESRCH; // Return process not found (search error) errno

  acquire_mutex(&p->lock);
  p->state = PROC_ZOMBIE; // Mark process as a zombie (to be cleaned up later)

  if (p->main_thread)                      // If process has a main thread
    thread_terminate(p->main_thread->tid); // Terminate the main thread

  release_mutex(&p->lock); // Release the per process mutex
  return 0;                // Return 0 (yayyyy)
}

int process_wait(pid_t pid, int *exit_code)
{
  thread_t *cur = kernel->current_thread;
  if (!cur)         // Should never happen
    return -EINVAL; // Return invalid argument errno

  pid_t cur_pid = cur->o_pid; // Owner process of the current running thread
  process_t *child = NULL;    // Initalize child process pointer

  lock_proc_table();

  int index;

  for (int i = 0; i < MAX_PROCS; ++i) // Iterate through every slot in the process table
  {
    process_t *p = process_table[i]; // Process p = i
    if (!p)
      continue;

    if (p->ppid != cur_pid) // If processes parent process ID doesn't match current running threads process ID
      continue;

    if (pid != -1 && p->pid != pid) // If specificed child PID doesn't match this processes PID (-1 means any child)
      continue;

    child = p; // If matching child is found, store it in the child process pointer
    index = i; // Store the index for later use (ignored if PID is -1)
    break;
  }

  unlock_proc_table();

  if (!child)       // If no child found
    return -ECHILD; // Return no child errno

  acquire_mutex(&child->lock);
  if (child->state == PROC_ZOMBIE || child->state == PROC_EXITED) // If child state is zombie or exited
  {
    int code = child->exit_code;
    child->state = PROC_EXITED; // Mark state as exited
    release_mutex(&child->lock);

    process_cleanup(child, index, pid, exit_code, code); // Call the process cleanup function
    return 0;
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

  // When thread resumes
  acquire_mutex(&child->lock);
  int code = child->exit_code; // Copy the exit code
  child->state = PROC_EXITED;  // Mark the child process as exited
  release_mutex(&child->lock);

  process_cleanup(child, index, pid, exit_code, code); // Call the process cleanup function
  return 0;
}

void init_processes(void)
{
  kprintf("Processes...");
  init_mutex(&process_table_lock);

  for (int i = 0; i < MAX_PROCS; ++i) // Iterate through every table slot
    process_table[i] = NULL;          // Set it to NULL

  process_t *kernel_process = (process_t *)kmalloc(sizeof(process_t)); // Allocate the kernel process
  memset(kernel_process, 0, sizeof(process_t));                        // Zero it out

  kernel_process->pid = 0;              // Process ID for the kernel process is 0
  kernel_process->ppid = 0;             // No parent (it is it's own parent?)
  kernel_process->state = PROC_RUNNING; // Set it as running
  kernel_process->address_space = 0;    // Kernel doesn't need an address space

  init_mutex(&kernel_process->lock);

  lock_proc_table();
  process_table[0] = kernel_process; // Append it to the list
  unlock_proc_table();
  kok();
}
