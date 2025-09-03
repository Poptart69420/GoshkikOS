#ifndef PAGINH_H_
#define PAGINH_H_

#include "../include/registers.h"
#include "kheap.h"

typedef struct page
{
  uint32_t present  : 1;
  uint32_t rw       : 1;
  uint32_t user     : 1;
  uint32_t accessed : 1;
  uint32_t dirty    : 1;
  uint32_t unused   : 1;
  uint32_t frame    : 1;
} page_t;

typedef struct page_table
{
  page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
  page_table_t *tables[1024];
  uint32_t physical_tables[1024];
  uint32_t physical_address;
} page_directory_t;

#endif // PAGINH_H_
