#include "paging.h"

static uint8_t page_table_pool[16 * 1024 * 1024] __attribute((aligned(PAGE_SIZE)));
static size_t page_table_pool_offset = 0;

static pml4_table_t pml4_table __attribute__((aligned(PAGE_SIZE)));

static page_table_t *alloc_page_table(void)
{
  size_t offset = (page_table_pool_offset + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

  if (offset + PAGE_SIZE > sizeof(page_table_pool)) {
    for (;;)
      __asm__ volatile ("hlt");
  }

  page_table_pool_offset = offset + PAGE_SIZE;
  page_table_t *page = (page_table_t *)&page_table_pool[offset];

  for (size_t i = 0; i < ENTRIES_PER_TABLE; ++i) {
    page->entries[i].present = 0;
    page->entries[i].writable = 0;
    page->entries[i].user_accessible = 0;
    page->entries[i].physical_address = 0;
  }
  return page;
}

static inline void write_cr3(uint64_t physical_address)
{
  __asm__ volatile ("mov %0, %%cr3\n" :: "r"(physical_address) : "memory");
}

static inline void enable_paging(void)
{
  uint64_t cr4;
  __asm__ volatile ("mov %%cr4, %0" : "=r"(cr4));
  cr4 |= (1UL << 5);
  __asm__ volatile ("mov %0, %%cr4" :: "r"(cr4));

  uint64_t cr0;
  __asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
  cr0 |= (1UL << 31);
  __asm__ volatile ("mov %0, %%cr0" :: "r"(cr0));
  __asm__ volatile ("sfence" ::: "memory");
}

static inline size_t plm4_index(uint64_t virtual_address)
{
  return (virtual_address >> 39) & 0x1FF;
}

static inline size_t pdpt_index(uint64_t virtual_address)
{
  return (virtual_address >> 30) & 0x1FF;
}

static inline size_t pd_index(uint64_t virtual_address)
{
  return (virtual_address >> 21) & 0x1FF;
}

static inline size_t pt_index(uint64_t virtual_address)
{
  return (virtual_address >> 12) & 0x1FF;
}

static pdpt_table_t *get_pdpt(pml4_table_t *pml4, size_t pml4_index)
{
  if (pml4->entries[pml4_index].present) {
    return (pdpt_table_t *)(uintptr_t)(pml4->entries[pml4_index].physical_address << 12);
  }

  pdpt_table_t *pdpt = (pdpt_table_t *)alloc_page_table();
  pml4->entries[pml4_index].present = 1;
  pml4->entries[pml4_index].writable = 1;
  pml4->entries[pml4_index].physical_address = ((uint64_t)(uintptr_t)pdpt) >> 12;

  return pdpt;
}

static pd_table_t *get_pd(pdpt_table_t *pdpt, size_t pdpt_index)
{
  if (pdpt->entries[pdpt_index].present) {
    return (pd_table_t *)(uintptr_t)(pdpt->entries[pdpt_index].physical_address << 12);
  }

  pd_table_t *pd = alloc_page_table();
  pdpt->entries[pdpt_index].present = 1;
  pdpt->entries[pdpt_index].writable = 1;
  pdpt->entries[pdpt_index].physical_address = ((uint64_t)(uintptr_t)pd) >> 12;

  return pd;
}

static page_table_t *get_pt(pd_table_t *pd, size_t pd_index)
{
  if (pd->entries[pd_index].present) {
    return (page_table_t *)(uintptr_t)(pd->entries[pd_index].physical_address << 12);
  }

  page_table_t *pt = alloc_page_table();
  pd->entries[pd_index].present = 1;
  pd->entries[pd_index].writable = 1;
  pd->entries[pd_index].physical_address = ((uint64_t)(uintptr_t)pd) >> 12;

  return pt;
}

static void map_page(uint64_t address)
{

}
