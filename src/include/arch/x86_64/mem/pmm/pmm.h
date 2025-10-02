#ifndef PMM_H_
#define PMM_H_

#include <arch/x86_64/asm/hcf.h>
#include <arch/x86_64/mem/mmu/mmu.h>
#include <global/global.h>
#include <klibc/kmem/mem.h>
#include <limine/limine.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

void init_pmm(void);
uintptr_t pmm_alloc_page(void);
void pmm_free_page(uintptr_t physical_address);
void *pmm_alloc_page_hhdm(void);

#endif // PMM_H_
