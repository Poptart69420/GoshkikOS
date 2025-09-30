#ifndef MEMMAP_H_
#define MEMMAP_H_

#include "../../limine.h"
#include "../drivers/vterm/kerror.h"
#include "../drivers/vterm/kok.h"
#include "../drivers/vterm/vterm.h"
#include "../include/global.h"
#include "../include/hcf.h"
#include "../limine_requests/limine_requests.h"
#include "mem.h"

void init_memmap(struct limine_memmap_response *response);
struct limine_memmap_entry *memmap_find_biggest_region(void);

#endif // MEMMAP_H_
