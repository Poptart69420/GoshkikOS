#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <limine/limine.h>
#include <stddef.h>
#include <stdint.h>
#include <vterm/vterm.h>

#define KSTACK_SIZE 0x8000
#define IST_STACK_SIZE 0x4000

extern uint64_t g_hhdm_offset;
extern struct limine_memmap_response *g_memmap;

#endif // GLOBAL_H_
