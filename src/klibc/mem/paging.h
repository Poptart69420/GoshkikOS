#ifndef PAGING_H_
#define PAGING_H_

#include <stdint.h>
#include <stddef.h>

#include "../../limine.h"
#include "mem.h"

#define PAGE_SIZE 4096ULL
#define MAX_PAGES 65536 * 8

#define PTE_PRESENT 0x1
#define PTE_WRITABLE 0x2
#define PTE_USER 0x4

#define ENTRIES_PER_TABLE 512ULL

#endif // PAGING_H_
