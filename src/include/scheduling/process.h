#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/cpu/gdt/gdt.h>
#include <arch/x86_64/cpu/idt/idt.h>
#include <arch/x86_64/cpu/timer/timer.h>
#include <arch/x86_64/mem/vmm/vmm.h>
#include <fs/vfs/vnode.t>
#include <klibc/cred/cred.h>
#include <klibc/types.h>
#include <scheduling/mutex.h>
#include <scheduling/thread.h>
#include <stdint.h>
#include <sys/time.h>

#define MAX_PROCS 256
#define PROC_NAME_MAX 64
#define MAX_FDS 256

typedef enum
{
  PROC_RUNNING,
  PROC_ZOMBIE,
  PROC_TERMED
} proc_state_t;

typedef struct process_t
{
  pid_t pid;
  pid_t ppid;
  char name[PROC_NAME_MAX];
  cred_t cred;
  mode_t umask;

  struct process_t *parent;
  struct process_t *next_sibling;
  struct process_t *first_child;

  struct thread_t *main_thread;

  int exit_code;
  mutex_t lock;
} process_t;

#endif // _PROCESS_H_
