#ifndef MEMMAP_H_
#define MEMMAP_H_

#include <arch/x86_64/asm/hcf.h>
#include <global/global.h>
#include <klibc/kmem/mem.h>
#include <limine/limine.h>
#include <limine/limine_requests.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

void init_mmu(struct limine_memmap_response *response);
struct limine_memmap_entry *memmap_find_biggest_region(void);

#endif // MEMMAP_H_
