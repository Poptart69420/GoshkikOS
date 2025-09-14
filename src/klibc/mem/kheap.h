#ifndef KHEAP_H_
#define KHEAP_H_

#include <stdint.h>
#include <stddef.h>
#include "../drivers/vterm/vterm.h"
#include "../include/global.h"
#include "../include/hcf.h"
#include "pmm.h"
#include "vmm.h"

#define HEAP_START      0xFFFF800010000000
#define HEAP_MAX        0xFFFF800020000000

typedef struct block_header
{
  size_t size;
  int free;
  struct block_header *next;
} block_header_t;

#endif // KHEAP_H_
