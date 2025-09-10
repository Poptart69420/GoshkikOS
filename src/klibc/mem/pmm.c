#include "pmm.h"

uint8_t *pmm_bitmap = NULL;
size_t pmm_bitmap_size = 0;
static uintptr_t managed_base = 0;
static size_t total_pages = 0;

#define BIT_SET(b, i)   ((b)[(i)/8] |=  (1 << ((i) % 8)))
#define BIT_CLEAR(b, i) ((b)[(i)/8] &= ~(1 << ((i) % 8)))
#define BIT_TEST(b, i)  ((b)[(i)/8] &   (1 << ((i) % 8)))

void init_pmm(void)
{
  struct limine_memmap_entry *biggest = memmap_find_biggest_region();

  if (!biggest) {
    vterm_print("PMM: No usable region found\n");
    for (;;) __asm__ volatile ("hlt");
  }

  managed_base    = biggest->base;
  total_pages     = biggest->length / PAGE_SIZE;
  pmm_bitmap_size = (total_pages + 7) / 8;

  pmm_bitmap = (uint8_t *)(g_hhdm_offset + managed_base);

  for (size_t i = 0; i < pmm_bitmap_size; ++i) {
    pmm_bitmap[i] = 0xFF;
  }

  size_t used_pages = (pmm_bitmap_size + PAGE_SIZE - 1) / PAGE_SIZE;
  for (size_t i = used_pages; i < total_pages; ++i) {
    BIT_CLEAR(pmm_bitmap, i);
  }

  vterm_print("PMM initalized\n");

}
