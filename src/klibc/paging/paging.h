#ifndef PAGING_H_
#define PAGING_H_

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096ULL
#define ENTRIES_PER_TABLE 512ULL
#define MAP_SIZE (4ULL * 1024 * 1024 * 1024)

#define PAGE_PRESENT 1
#define PAGE_WRITABLE 1

typedef struct page_table_entry {
  uint64_t present : 1;
  uint64_t writable : 1;
  uint64_t user_accessible : 1;
  uint64_t write_through : 1;
  uint64_t cache_disable : 1;
  uint64_t accessed : 1;
  uint64_t dirty : 1;
  uint64_t page_size : 1;
  uint64_t global : 1;  uint64_t ignored : 3;
  uint64_t physical_address : 40;
  uint64_t available : 11;
  uint64_t nx : 1;
} __attribute__((packed)) page_table_entry_t;

typedef struct page_table {
  page_table_entry_t entries[ENTRIES_PER_TABLE];
} __attribute__((aligned(PAGE_SIZE))) page_table_t;

typedef page_table_t pml4_table_t;
typedef page_table_t pdpt_table_t;
typedef page_table_t pd_table_t;

void paging_map_kernel(uint64_t k_physical_base, uint64_t k_virtual_base, uint64_t k_physical_size, uint64_t identity_map_bytes);

#endif // PAGING_H_
