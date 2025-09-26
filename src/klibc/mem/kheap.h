#ifndef KHEAP_H_
#define KHEAP_H_

#include "../drivers/vterm/vterm.h"
#include "../include/global.h"
#include "../include/hcf.h"
#include "pmm.h"
#include "vmm.h"
#include <stddef.h>
#include <stdint.h>

#define HEAP_START 0xFFFF800010000000

typedef struct block_header {
    size_t size;
    int free;
    struct block_header *next;
} block_header_t;

size_t init_kheap(void);
void *kmalloc(size_t size);
void *kzalloc(size_t size);
void *kcalloc(size_t n, size_t size);
void kfree(void *ptr);

#endif // KHEAP_H_
