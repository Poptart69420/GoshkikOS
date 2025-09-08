#include "paging.h"

static uint8_t page_table_pool[16 * 1024 * 1024] __attribute__((aligned(PAGE_SIZE)));
static size_t page_table_pool_offset = 0;
static int64_t physical_offset = 0;

static inline uint64_t virtual_to_physical(void *ptr) {
    return (uint64_t)(uintptr_t)ptr + (uint64_t)physical_offset;
}

static pml4_table_t *pml4_table __attribute__((aligned(PAGE_SIZE)));

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
    page->entries[i].write_through = 0;
    page->entries[i].cache_disable = 0;
    page->entries[i].accessed = 0;
    page->entries[i].dirty = 0;
    page->entries[i].page_size = 0;
    page->entries[i].global = 0;
    page->entries[i].ignored = 0;
    page->entries[i].physical_address = 0;
    page->entries[i].available = 0;
    page->entries[i].nx = 0;
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

static inline size_t pml4_index(uint64_t virtual_address)
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
  uint64_t pdpt_physical = virtual_to_physical(pdpt);
  pml4->entries[pml4_index].present = 1;
  pml4->entries[pml4_index].writable = 1;
  pml4->entries[pml4_index].physical_address = pdpt_physical >> 12;

  return pdpt;
}

static pd_table_t *get_pd(pdpt_table_t *pdpt, size_t pdpt_index)
{
  if (pdpt->entries[pdpt_index].present) {
    return (pd_table_t *)(uintptr_t)(pdpt->entries[pdpt_index].physical_address << 12);
  }

  pd_table_t *pd = alloc_page_table();
  uint64_t pd_physical = virtual_to_physical(pd);
  pdpt->entries[pdpt_index].present = 1;
  pdpt->entries[pdpt_index].writable = 1;
  pdpt->entries[pdpt_index].physical_address = pd_physical >> 12;

  return pd;
}

static page_table_t *get_pt(pd_table_t *pd, size_t pd_index)
{
  if (pd->entries[pd_index].present) {
    return (page_table_t *)(uintptr_t)(pd->entries[pd_index].physical_address << 12);
  }

  page_table_t *pt = alloc_page_table();
  uint64_t pt_physical = virtual_to_physical(pt);
  pd->entries[pd_index].present = 1;
  pd->entries[pd_index].writable = 1;
  pd->entries[pd_index].physical_address = pt_physical >> 12;

  return pt;
}

static void map_page(uint64_t virtual_address, uint64_t physical_address)
{
  size_t i_pml4 = pml4_index(virtual_address);
  size_t i_pdpt = pdpt_index(virtual_address);
  size_t i_pd = pd_index(virtual_address);
  size_t i_pt = pt_index(virtual_address);

  pdpt_table_t *pdpt = get_pdpt(pml4_table, i_pml4);
  pd_table_t *pd = get_pd(pdpt, i_pdpt);
  page_table_t *pt = get_pt(pd, i_pd);

  pt->entries[i_pt].present = 1;
  pt->entries[i_pt].writable = 1;
  pt->entries[i_pt].physical_address = physical_address >> 12;
}

static void map_range(uint64_t virtual_start, uint64_t physical_start, uint64_t size)
{
  uint64_t end = virtual_start + size;
  uint64_t virtual_address = virtual_start;
  uint64_t physical_address = physical_start;

  for (; virtual_address < end; virtual_address += PAGE_SIZE, physical_address += PAGE_SIZE) {
    map_page(virtual_address, physical_address);
  }
}

void paging_map_kernel(uint64_t k_physical_base, uint64_t k_virtual_base, uint64_t k_physical_size, uint64_t identity_map_bytes)
{
  physical_offset = (int64_t)k_physical_base - (int64_t)k_virtual_base;

  pml4_table = (pml4_table_t *)alloc_page_table();
  uint64_t pml4_physical = virtual_to_physical(pml4_table);
  map_range(k_virtual_base, k_physical_base, k_physical_size);

  if (identity_map_bytes > 0) {
    map_range(0, 0, identity_map_bytes);
  }

  write_cr3(pml4_physical);
  enable_paging();
}
