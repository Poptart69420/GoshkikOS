#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "limine.h"
#include "klibc/mem/mem.h"
#include "klibc/gdt/gdt.h"
#include "klibc/isr/isr.h"
#include "klibc/pic/pic.h"
#include "klibc/drivers/vterm/vterm.h"
#include "klibc/pit/pit.h"
#include "klibc/drivers/ps2/ps2.h"
#include "klibc/drivers/ps2/kbd.h"
#include "klibc/timer/timer.h"
#include "klibc/include/global.h"
#include "klibc/mem/memmap.h"
#include "klibc/mem/pmm.h"
#include "klibc/mem/vmm.h"
#include "klibc/limine_requests/limine_requests.h"
#include "klibc/include/hcf.h"
#include "klibc/mem/kheap.h"
#include "klibc/drivers/fs/vfs.h"
#include "klibc/drivers/fs/ramfs/ramfs.h"
#include "klibc/drivers/fs/tar/tar.h"

typedef struct kernel_table_struct
{
  struct limine_framebuffer_response *framebuffer;
  struct limine_hhdm_response        *hhdm;
  struct limine_memmap_response      *memmap;
  struct limine_module_response      *module;
} kernel_table;

extern kernel_table *kernel;

#endif // KERNEL_H_
