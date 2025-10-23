#include <arch/x86_64/mem/vmm/vmm.h>

static uintptr_t current_pml4 = 0;

uint64_t get_address_space(void)
{
  uint64_t cr3;
  __asm__ volatile(
      "mov rax, cr3\n\t"
      : "=a"(cr3));
  return (cr3 + g_hhdm_offset);
}

void *physical_to_virtual(uintptr_t physical)
{
  return (void *)(g_hhdm_offset + physical);
}

uintptr_t virtual_to_physical(void *virtual)
{
  return (uintptr_t)virtual - g_hhdm_offset;
}

static inline uintptr_t read_cr3(void)
{
  uintptr_t cr3;
  __asm__ volatile(
      "mov rax, cr3\n\t"
      : "=a"(cr3));
  return cr3;
}

static inline void invlpg(uintptr_t address)
{
  __asm__ volatile(
      "invlpg [rax]\n\t"
      :
      : "a"(address)
      : "memory");
}

static uintptr_t alloc_table(void)
{
  uintptr_t physical = pmm_alloc_page();

  if (!physical)
  {
    vterm_print("VMM:   Out Of Memory\n");
    hcf();
  }

  memset(physical_to_virtual(physical), 0, PAGE_SIZE_VMM);
  return physical;
}

static uint64_t *walk(uintptr_t virtual_address, int create)
{
  uint64_t *pml4 = (uint64_t *)physical_to_virtual(current_pml4);
  uint64_t *pml4e = &pml4[PML4_INDEX(virtual_address)];

  if (!(*pml4e & VMM_PRESENT))
  {
    if (!create)
      return NULL;
    uintptr_t new = alloc_table();
    *pml4e = (new & PAGE_MASK) | VMM_PRESENT | VMM_WRITE;
  }

  uint64_t *pdpt = (uint64_t *)physical_to_virtual((*pml4e) & PAGE_MASK);
  uint64_t *pdpte = &pdpt[PDPT_INDEX(virtual_address)];
  if (!(*pdpte & VMM_PRESENT))
  {
    if (!create)
      return NULL;
    uintptr_t new = alloc_table();
    *pdpte = (new & PAGE_MASK) | VMM_PRESENT | VMM_WRITE;
  }

  uint64_t *pd = (uint64_t *)physical_to_virtual((*pdpte) & PAGE_MASK);
  uint64_t *pde = &pd[PD_INDEX(virtual_address)];
  if (!(*pde & VMM_PRESENT))
  {
    if (!create)
      return NULL;
    uintptr_t new = alloc_table();
    *pde = (new & PAGE_MASK) | VMM_PRESENT | VMM_WRITE;
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
  if (pte && (*pte & VMM_PRESENT))
  {
    *pte = 0;
    invlpg(virtual);
  }
}

uintptr_t vmm_resolve(uintptr_t virtual)
{
  const uint64_t *pte = walk(virtual, 0);
  if (!pte || !(*pte & VMM_PRESENT))
  {
    return 0;
  }

  return ((*pte) & PAGE_MASK) | (virtual & (PAGE_SIZE_VMM - 1));
}

void vmm_load_cr3(uintptr_t physical_address)
{
  current_pml4 = physical_address & PAGE_MASK;
  __asm__ volatile(
      "mov cr3, rax\n\t"
      :
      : "a"(current_pml4)
      : "memory");
}

void init_vmm(void)
{
  kprintf("VMM...");
  const uintptr_t old_cr3_physical = read_cr3() & PAGE_MASK;
  const uint64_t *old_pml4 = (uint64_t *)physical_to_virtual(old_cr3_physical);

  current_pml4 = alloc_table();
  uint64_t *new_pml4 = (uint64_t *)physical_to_virtual(current_pml4);

  for (size_t i = 256; i < ENTRIES_PER_TABLE; ++i)
  {
    new_pml4[i] = old_pml4[i];
  }

  for (uint64_t i = 0; i < g_memmap->entry_count; ++i)
  {
    const struct limine_memmap_entry *e = g_memmap->entries[i];

    if (e->type == LIMINE_MEMMAP_USABLE)
    {
      uintptr_t end = e->base + e->length;
      for (uintptr_t address = e->base; address < end;
           address += PAGE_SIZE_VMM)
      {
        vmm_map(address, address, VMM_WRITE);
      }
    }
  }

  extern uint8_t *pmm_bitmap;
  extern size_t pmm_bitmap_size;

  if (pmm_bitmap && pmm_bitmap_size)
  {
    uintptr_t bitmap_physical = virtual_to_physical(pmm_bitmap);

    for (size_t i = 0; i < pmm_bitmap_size; i += PAGE_SIZE_VMM)
    {
      vmm_map(bitmap_physical + g_hhdm_offset + i, bitmap_physical + i,
              VMM_WRITE);
    }
  }

  vmm_load_cr3(current_pml4);
  kok();
}

uintptr_t create_address_space(void)
{
  uintptr_t new_pml4_phys = pmm_alloc_page();
  if (!new_pml4_phys)
  {
    vterm_print("VMM:   Out Of Memory (create_address_space)\n");
    hcf();
  }

  memset(physical_to_virtual(new_pml4_phys), 0, PAGE_SIZE_VMM);

  uint64_t *kernel_pml4 = (uint64_t *)physical_to_virtual(current_pml4);
  uint64_t *new_pml4 = (uint64_t *)physical_to_virtual(new_pml4_phys);

  for (size_t i = 256; i < ENTRIES_PER_TABLE; ++i)
  {
    new_pml4[i] = kernel_pml4[i];
  }

  return new_pml4_phys;
}
