#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <stdint.h>
#include <stddef.h>
#include "../drivers/vterm/vterm.h"
#include "../../limine.h"

extern uint64_t g_hhdm_offset;
extern struct limine_memmap_response *g_memmap;

#endif // GLOBAL_H_
