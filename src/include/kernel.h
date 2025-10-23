#ifndef KERNEL_H_
#define KERNEL_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/asm/hcf.h>
#include <arch/x86_64/cpu/isr/isr.h>
#include <arch/x86_64/cpu/pic/pic.h>
#include <arch/x86_64/cpu/pit/pit.h>
#include <arch/x86_64/cpu/timer/timer.h>
#include <arch/x86_64/mem/mmu/mmu.h>
#include <arch/x86_64/mem/pmm/pmm.h>
#include <arch/x86_64/mem/vmm/vmm.h>
#include <arch/x86_64/ps2/keyboard/kbd.h>
#include <arch/x86_64/ps2/ps2.h>
#include <arch/x86_64/serial/serial.h>
#include <fs/tmpfs/tmpfs.h>
#include <fs/vfs/vfs.h>
#include <global/global.h>
#include <klibc/kmem/kheap.h>
#include <klibc/kmem/slab.h>
#include <limine/limine_requests.h>
#include <scheduling/scheduler.h>
#include <scheduling/syscall.h>
#include <stdbool.h>
#include <sys/time.h>
#include <vterm/vterm.h>

// To-do: Create a testing interface
// Tests

typedef struct kernel_table_struct
{
  // Kernel Limine reponses
  struct limine_framebuffer_response *framebuffer;
  struct limine_hhdm_response *hhdm;
  struct limine_memmap_response *memmap;
  struct limine_module_response *module;

  struct system_time current_time;

  // Kernel processes & scheduling
  process_t *current_process;
  bool task_switch;
  uint64_t created_process_count;
} kernel_table;

extern kernel_table *kernel;
extern struct timeval time;

#endif // KERNEL_H_
