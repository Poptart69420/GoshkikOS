#ifndef KHEAP_H_
#define KHEAP_H_

#include <arch/x86_64/asm/hcf.h>
#include <arch/x86_64/mem/pmm/pmm.h>
#include <arch/x86_64/mem/vmm/vmm.h>
#include <global/global.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

#define HEAP_START 0xFFFF800010000000

typedef struct block_header
{
  size_t size;
  int free;
  struct block_header *next;
} block_header_t;

size_t init_kheap(void);

__attribute__((malloc)) void *kmalloc(size_t size);

void *kzalloc(size_t size);

void *kcalloc(size_t n, size_t size);

void kfree(void *ptr);

#endif // KHEAP_H_
