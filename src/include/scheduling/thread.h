#ifndef THREAD_H_
#define THREAD_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/cpu/idt/idt.h>
#include <arch/x86_64/cpu/timer/timer.h>
#include <global/global.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_THREADS 256
#define TSTACK_SIZE 0x4000

typedef enum
{
  THREAD_READY = 0,
  THREAD_RUNNING,
  THREAD_BLOCKED,
  THREAD_TERMED
} thread_state_t;

typedef enum
{
  THREAD_RING_0 = 0, // Kernel
  THREAD_RING_3 = 3  // User
} thread_privilege_t;

typedef enum
{
  THREAD_PRIO_LOW = 0,
  THREAD_PRIO_BG,
  THREAD_PRIO_NORMAL,
  THREAD_PRIO_HIGH,
  THREAD_PRIO_IMPORTANT,
  THREAD_PRIO_IMMEDIATE,
  PRIORITY_LEVELS
} thread_priority_t;

typedef struct thread
{
  uint32_t tid;                 // Thread ID
  uint32_t o_pid;               // Owner PID
  thread_state_t state;         // State
  thread_privilege_t privilege; // Ring level
  thread_priority_t priority;   // Priority
  context_t context;            // CPU frame
  uint64_t stack_base;          // Stack base address
  uint64_t stack_size;          // Stack size
  uint64_t kernel_stack;        // Kernel stack
  uint32_t time_slice;          // Time slice
  uint32_t time_used;           // Time used in slice
  int cooldown;                 // Cooldown
  void *user_data;              // User data
  struct thread *next;          // Next thread in queue
} thread_t;

typedef void (*thread_function_t)(int argc, char **argv);

void init_threading(void);
void thread_block(uint32_t tid);
int thread_unlock(uint32_t tid);
thread_t *thread_create(thread_function_t function, int argc, char **argv, thread_privilege_t privilege, thread_priority_t priority, uint32_t owner_pid);

#endif // THREAD_H_
