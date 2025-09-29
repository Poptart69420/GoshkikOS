#include "pmm.h"

uint8_t *pmm_bitmap = NULL;
size_t pmm_bitmap_size = 0;
static uintptr_t managed_base = 0;
static size_t total_pages = 0;

#define BIT_SET(b, i) ((b)[(i) / 8] |= (1 << ((i) % 8)))
#define BIT_CLEAR(b, i) ((b)[(i) / 8] &= ~(1 << ((i) % 8)))
#define BIT_TEST(b, i) ((b)[(i) / 8] & (1 << ((i) % 8)))

void init_pmm(void) {
    const struct limine_memmap_entry *biggest = memmap_find_biggest_region();

    vterm_print("PMM...");

    if (!biggest) {
        kerror("No usable region found");
        hcf();
    }

    managed_base = biggest->base;
    total_pages = biggest->length / PAGE_SIZE;
    pmm_bitmap_size = (total_pages + 7) / 8;

    pmm_bitmap = (uint8_t *)(g_hhdm_offset + managed_base);

    for (size_t i = 0; i < pmm_bitmap_size; ++i) {
        pmm_bitmap[i] = 0xFF;
    }

    size_t used_pages = (pmm_bitmap_size + PAGE_SIZE - 1) / PAGE_SIZE;
    for (size_t i = used_pages; i < total_pages; ++i) {
        BIT_CLEAR(pmm_bitmap, i);
    }

    kok();
}

uintptr_t pmm_alloc_page(void) {
    for (size_t i = 0; i < total_pages; ++i) {
        if (!BIT_TEST(pmm_bitmap, i)) {
            BIT_SET(pmm_bitmap, i);
            return managed_base + i * PAGE_SIZE;
        }
    }

    return 0;
}

void pmm_free_page(uintptr_t physical_address) {
    if (physical_address < managed_base) {
        return;
    }

    size_t i = (physical_address - managed_base) / PAGE_SIZE;
    if (i < total_pages) {
        BIT_CLEAR(pmm_bitmap, i);
    }
}

void *pmm_alloc_page_hhdm(void) {
    uintptr_t physical = pmm_alloc_page();

    if (!physical) {
        return NULL;
    }

    return (void *)(g_hhdm_offset + physical);
}
