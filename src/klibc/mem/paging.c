#include "paging.h"

volatile struct limine_executable_file_request executable_file_request =
{
  .id = LIMINE_EXECUTABLE_FILE_REQUEST,
  .revision = 0
};

volatile struct limine_hhdm_request hhdm_request =
{
  .id = LIMINE_HHDM_REQUEST,
  .revision = 0
};

[[gnu::aligned(PAGE_SIZE)]] uint64_t pml4[ENTRIES_PER_TABLE];
[[gnu::aligned(PAGE_SIZE)]] uint64_t pdpt[ENTRIES_PER_TABLE];
[[gnu::aligned(PAGE_SIZE)]] uint64_t pd[ENTRIES_PER_TABLE];
[[gnu::aligned(PAGE_SIZE)]] uint64_t pt[ENTRIES_PER_TABLE];

[[gnu::aligned(PAGE_SIZE)]] uint64_t pdpt_hhdm[ENTRIES_PER_TABLE];
[[gnu::aligned(PAGE_SIZE)]] uint64_t pd_hhdm[ENTRIES_PER_TABLE];
[[gnu::aligned(PAGE_SIZE)]] uint64_t pt_hhdm[ENTRIES_PER_TABLE];

static inline size_t pml4_index(uint64_t va) { return (va >> 39) & 0x1FF; }
static inline size_t pdpt_index(uint64_t va) { return (va >> 30) & 0x1FF; }
static inline size_t pd_index(uint64_t va)   { return (va >> 21) & 0x1FF; }
static inline size_t pt_index(uint64_t va)   { return (va >> 12) & 0x1FF; }

void load_pages(void)
{
  memset(pml4,      0, PAGE_SIZE);
  memset(pdpt,      0, PAGE_SIZE);
  memset(pd,        0, PAGE_SIZE);
  memset(pt,        0, PAGE_SIZE);
  memset(pdpt_hhdm, 0, PAGE_SIZE);
  memset(pd_hhdm,   0, PAGE_SIZE);
  memset(pt_hhdm,   0, PAGE_SIZE);

  pml4[0]   = (uint64_t)pdpt | PTE_PRESENT | PTE_WRITABLE;
  pdpt[0]   = (uint64_t)pd   | PTE_PRESENT | PTE_WRITABLE;
  pd[0]     = (uint64_t)pt   | PTE_PRESENT | PTE_WRITABLE;

}
