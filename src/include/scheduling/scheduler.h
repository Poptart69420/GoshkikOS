#ifndef SCHEDULAR_H_
#define SCHEDULAR_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/cpu/gdt/gdt.h>
#include <arch/x86_64/cpu/idt/idt.h>
#include <arch/x86_64/cpu/timer/timer.h>
#include <arch/x86_64/mem/vmm/vmm.h>
#include <arch/x86_64/selectors/selectors.h>
#include <global/global.h>
#include <klibc/cred/cred.h>
#include <klibc/kmem/list.h>
#include <klibc/types.h>
#include <scheduling/mutex.h>
#include <stdint.h>
#include <sys/time.h>

#define KSTACK_TOP(kstack) (((kstack) + KSTACK_SIZE) & ~0xFUL)

#define PROCESS_FLAG_PRESENT 0x01
#define PROCESS_FLAG_ZOMBIE 0x02
#define PROCESS_FLAG_RUN 0x04
#define PROCESS_FLAG_WAIT 0x08
#define PROCESS_FLAG_INTR 0x10
#define PROCESS_FLAG_BLOCKED 0x20
#define PROCESS_FLAG_STOPPED 0x40

typedef struct process_struct_t
{
  fault_frame_t context;
  uintptr_t kernel_stack;
  uintptr_t heap_start;
  uintptr_t heap_end;
  uint64_t address_space;
  atomic_int flags;
  mutex_t sig_lock;
  struct timeval wakeup_time;
  long exit_status;

  cred_t cred;
  pid_t pid;
  pid_t group;
  pid_t sid;
  pid_t wait_for;
  mode_t umask;

  list_t *child;
  list_t *mem_seg;

  struct process_struct_t *snext;
  struct process_struct_t *next;
  struct process_struct_t *prev;
  struct process_struct_t *parent;
  struct process_struct_t *waker;
} process_t;

void init_kernel_task(void);
process_t *get_current_process(void);

#endif // SHEDULAR_H_
