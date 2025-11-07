#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/cpu/gdt/gdt.h>
#include <arch/x86_64/cpu/idt/idt.h>
#include <arch/x86_64/cpu/timer/timer.h>
#include <arch/x86_64/mem/vmm/vmm.h>
#include <fs/vfs/vnode.h>
#include <klibc/cred/cred.h>
#include <scheduling/mutex.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>

#define MAX_PROCS 1024
#define MAX_FD 32

#define FD_READ 0x01
#define FD_WRITE 0x02
#define FD_APPEND 0x04
#define FD_NONBLOCK 0x08

typedef struct file_table file_table_t;
typedef void (*thread_function_t)(int argc, char **argv);

typedef enum
{
  PROC_READY = 0,
  PROC_RUNNING,
  PROC_BLOCKED,
  PROC_ZOMBIE,
  PROC_EXITED
} process_state_t;

typedef struct
{
  struct vnode_t *node;
  uint64_t offset;
  uint64_t present;
  uint64_t flags;
} file_descriptor_t;

typedef struct process_t
{
  pid_t pid;             // Process ID
  pid_t ppid;            // Parent process ID
  cred_t cred;           // Credentials
  process_state_t state; // Current process state (running, zombie, terminated)
  mode_t umask;          // Process file creation mask (default permissions for new files)

  struct process_t *next_sibling; // Next process in chain

  thread_t *main_thread; // Initial/leading thread
  thread_t *wait_head;   // Waiting thread list head
  thread_t *wait_tail;   // Waiting thread list tail

  struct vnode_t *cwd;     // Current working directory
  char cwd_path[PATH_MAX]; // Canonical cwd (NULL terminated)

  uintptr_t address_space;       // Physical CR3/PML4
  file_descriptor_t fds[MAX_FD]; // File descriptors

  int exit_code; // Status for parent
  mutex_t lock;  // Mutex lock
} process_t;

void init_processes(void);
int process_remove_thread(process_t *p, thread_t *t);
process_t *process_from_pid(pid_t pid);
int process_add_thread(process_t *p, thread_t *t);
void wake_waiters(process_t *p);
process_t *process_create(thread_function_t entry, int argc, char **argv, pid_t ppid, int is_userspace);

#endif // _PROCESS_H_
