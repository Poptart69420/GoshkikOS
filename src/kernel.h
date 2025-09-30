#ifndef KERNEL_H_
#define KERNEL_H_

#include "klibc/arch/x86_64/asm.h"
#include "klibc/drivers/fs/tarfs/tarfs.h"
#include "klibc/drivers/fs/tmpfs/tmpfs.h"
#include "klibc/drivers/fs/vfs.h"
#include "klibc/drivers/ps2/kbd.h"
#include "klibc/drivers/ps2/ps2.h"
#include "klibc/drivers/vterm/kerror.h"
#include "klibc/drivers/vterm/kok.h"
#include "klibc/drivers/vterm/vterm.h"
#include "klibc/gdt/gdt.h"
#include "klibc/include/global.h"
#include "klibc/include/hcf.h"
#include "klibc/isr/isr.h"
#include "klibc/limine_requests/limine_requests.h"
#include "klibc/mem/kheap.h"
#include "klibc/mem/mem.h"
#include "klibc/mem/memmap.h"
#include "klibc/mem/pmm.h"
#include "klibc/mem/vmm.h"
#include "klibc/pic/pic.h"
#include "klibc/pit/pit.h"
#include "klibc/timer/timer.h"
#include "limine.h"

typedef struct kernel_table_struct {
    struct limine_framebuffer_response *framebuffer;
    struct limine_hhdm_response *hhdm;
    struct limine_memmap_response *memmap;
    struct limine_module_response *module;
    vfs_mount_point_t *first_mount_point;
} kernel_table;

extern kernel_table *kernel;

#endif // KERNEL_H_
