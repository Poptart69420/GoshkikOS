#ifndef PMM_H_
#define PMM_H_

#include <stdint.h>
#include <stddef.h>
#include "../drivers/vterm/vterm.h"
#include "mem.h"
#include "../../limine.h"
#include "memmap.h"
#include "../include/global.h"
#include "../include/hcf.h"

void init_pmm(void);
uintptr_t pmm_alloc_page(void);
void pmm_free_page(uintptr_t physical_address);
void *pmm_alloc_page_hhdm(void);

#endif // PMM_H_
