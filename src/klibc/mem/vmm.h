#ifndef VMM_H_
#define VMM_H_

#include <stdint.h>
#include <stddef.h>
#include "../drivers/vterm/vterm.h"
#include "mem.h"
#include "../../limine.h"
#include "../include/global.h"
#include "memmap.h"
#include "pmm.h"

#define PAGE_SIZE_VMM 0x1000
#define VMM_PRESENT   (1ull << 0)
#define VMM_WRITE     (1ull << 1)
#define VMM_USER      (1ull << 2)
#define VMM_WRITE_THR (1ull << 3)
#define VMM_CACHE_DIS (1ull << 4)
#define VMM_ACCESSED  (1ull << 5)
#define VMM_DIRTY     (1ull << 6)
#define VMM_HUGE      (1ull << 7)
#define VMM_GLOBAL    (1ull << 8)
#define VMM_NX        (1ull << 63)

#define ENTRIES_PER_TABLE 512
#define PAGE_MASK 0x000FFFFFFFFFF000ULL

#define PML4_INDEX(x) (((x) >> 39) & 0x1FF)
#define PDPT_INDEX(x) (((x) >> 30) & 0x1FF)
#define PD_INDEX(x)   (((x) >> 21) & 0x1FF)
#define PT_INDEX(x)   (((x) >> 12) & 0x1FF)

#endif // VMM_H_
