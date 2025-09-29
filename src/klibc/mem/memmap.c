#include "memmap.h"

void init_memmap(struct limine_memmap_response *response) {
    vterm_print("Memmap...");

    g_memmap = response;

    if (!g_memmap) {
        kerror("No memmap provided");
        hcf();
    }

    kok();
}

struct limine_memmap_entry *memmap_find_biggest_region(void) {
    struct limine_memmap_entry *biggest = NULL;

    for (uint64_t i = 0; i < g_memmap->entry_count; ++i) {
        struct limine_memmap_entry *entry = g_memmap->entries[i];
        if (entry->type != LIMINE_MEMMAP_USABLE)
            continue;

        if (!biggest || entry->length > biggest->length) {
            biggest = entry;
        }
    }

    return biggest;
}
