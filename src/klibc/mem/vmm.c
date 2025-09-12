#include "vmm.h"

static uintptr_t current_pml4 = 0;

static inline void *physical_to_virtual(uintptr_t physical)
{
  return (void *)(g_hhdm_offset + physical);
}

static inline uintptr_t read_cr3(void)
{
  uintptr_t cr3;
  __asm__ volatile ("mov %%cr3, %0" : "=r"(cr3));
  return cr3;
}

static inline void invlpg(uintptr_t address)
{
  __asm__ volatile ("invlpg (%0)" :: "r"(address) : "memory");
}

static uintptr_t alloc_table(void)
{
  uintptr_t physical = pmm_alloc_page();

  if (!physical) {
    vterm_print("VMM: Out Of Memory\n");

    hcf();
  }

  memset(physical_to_virtual(physical), 0, PAGE_SIZE);
  return physical;
}
