#ifndef VMM_H_
#define VMM_H_

#include <arch/x86_64/asm/hcf.h>
#include <arch/x86_64/mem/mmu/mmu.h>
#include <arch/x86_64/mem/pmm/pmm.h>
#include <global/global.h>
#include <klibc/kmem/mem.h>
#include <limine/limine.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

#define PAGE_SIZE_VMM 0x1000
#define VMM_PRESENT (1ull << 0)
#define VMM_WRITE (1ull << 1)
#define VMM_USER (1ull << 2)
#define VMM_WRITE_THR (1ull << 3)
#define VMM_CACHE_DIS (1ull << 4)
#define VMM_ACCESSED (1ull << 5)
#define VMM_DIRTY (1ull << 6)
#define VMM_HUGE (1ull << 7)
#define VMM_GLOBAL (1ull << 8)
#define VMM_NX (1ull << 63)

#define ENTRIES_PER_TABLE 512
#define PAGE_MASK 0x000FFFFFFFFFF000ULL

#define PML4_INDEX(x) (((x) >> 39) & 0x1FF)
#define PDPT_INDEX(x) (((x) >> 30) & 0x1FF)
#define PD_INDEX(x) (((x) >> 21) & 0x1FF)
#define PT_INDEX(x) (((x) >> 12) & 0x1FF)

uint64_t get_address_space(void);
void init_vmm(void);
uintptr_t vmm_resolve(uintptr_t virtual);
void vmm_map(uintptr_t virtual, uintptr_t physical, uint64_t flags);
void vmm_unmap(uintptr_t virtual);
void *physical_to_virtual(uintptr_t physical);
uintptr_t virtual_to_physical(void *virtual);

#endif // VMM_H_
