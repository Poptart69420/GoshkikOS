#ifndef MEMMAP_H_
#define MEMMAP_H_

#include <stdint.h>
#include <stddef.h>
#include "../../limine.h"
#include "../drivers/vterm/vterm.h"
#include "mem.h"
#include "../include/global.h"
#include "../limine_requests/limine_requests.h"

void init_memmap(struct limine_memmap_response *response);
struct limine_memmap_entry *memmap_find_biggest_region(void);

#endif // MEMMAP_H_
