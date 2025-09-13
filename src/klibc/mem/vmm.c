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

static uint64_t *walk(uintptr_t virtual_address, int create)
{
  uint64_t *pml4 = (uint64_t *)physical_to_virtual(current_pml4);
  uint64_t *pml4e = &pml4[PML4_INDEX(virtual_address)];

  if (!(*pml4e & VMM_PRESENT)) {
    if (!create) {
      return NULL;
    } else {
      uintptr_t new = alloc_table();
      *pml4e = (new & PAGE_MASK) | VMM_PRESENT | VMM_WRITE;
    }
  }

  uint64_t *pdpt = (uint64_t *)physical_to_virtual(virtual_address);

  uint64_t *pdpte = &pdpt[PDPT_INDEX(virtual_address)];
  if (!(*pdpte & VMM_PRESENT)) {
    if (!create) {
      return NULL;
    } else {
      uintptr_t new = alloc_table();
      *pdpte = (new & PAGE_MASK) | VMM_PRESENT | VMM_WRITE;
    }
  }

  uint64_t *pd = (uint64_t *)physical_to_virtual((*pdpt) & PAGE_MASK);

  uint64_t *pde = &pd[PD_INDEX(virtual_address)];
  if (!(*pde & VMM_PRESENT)) {
    if (!create) {
      return NULL;
    } else {
      uintptr_t new = alloc_table();
      *pde = (new & PAGE_MASK) | VMM_PRESENT | VMM_WRITE;
    }
  }

  uint64_t *pt = (uint64_t *)physical_to_virtual((*pde) & PAGE_MASK);

  return &pt[PT_INDEX(virtual_address)];
}

void vmm_map(uintptr_t virtual, uintptr_t physical, uint64_t flags)
{
  uint64_t *pte = walk(virtual, 1);
  *pte = (physical & PAGE_MASK) | flags | VMM_PRESENT;
  invlpg(virtual);
}

void vmm_unmap(uintptr_t virtual)
{
  uint64_t *pte = walk(virtual, 0);
  if (pte && (*pte & VMM_PRESENT)) {
    *pte = 0;
    invlpg(virtual);
  }
}

uintptr_t vmm_resolve(uintptr_t virtual)
{
  const uint64_t *pte = walk(virtual, 0);
  if (!pte || !(*pte & VMM_PRESENT)) {
    return 0;
  }

  return ((*pte) & PAGE_MASK) | (virtual & (PAGE_SIZE - 1));
}

void vmm_load_cr3(uintptr_t physical_address)
{
  current_pml4 = physical_address & PAGE_MASK;
  __asm__ volatile ("mov %0, %%cr3" :: "r"(current_pml4) : "memory");
}
