#include <klibc/kmem/kheap.h>

static uintptr_t heap_current = HEAP_START;
static block_header_t *free_list = NULL;

size_t init_kheap(void)
{
  heap_current = HEAP_START;

  uintptr_t physical = pmm_alloc_page();

  kprintf("Kheap...");

  if (!physical)
  {
    kerror("Failed to initialize kheap");
    return 0;
  }

  vmm_map(heap_current, physical, VMM_PRESENT | VMM_WRITE);
  free_list = (block_header_t *)heap_current;
  free_list->size = PAGE_SIZE_VMM - sizeof(block_header_t);
  free_list->free = 1;
  free_list->next = NULL;

  kok();
  return free_list->size;
}

static void split_block(block_header_t *block, size_t size)
{
  if (block->size <= size + sizeof(block_header_t))
    return;

  block_header_t *new_block =
      (block_header_t *)((uintptr_t)block + sizeof(block_header_t) + size);
  new_block->size = block->size - size - sizeof(block_header_t);
  new_block->free = 1;
  new_block->next = block->next;

  block->size = size;
  block->next = new_block;
}

static void *request_more_memory(size_t size)
{
  if (size > SIZE_MAX - sizeof(block_header_t))
    return NULL;

  size_t total_size = size + sizeof(block_header_t);
  total_size = (total_size + PAGE_SIZE_VMM - 1) & ~(PAGE_SIZE_VMM - 1);

  uintptr_t alloc_base = heap_current;
  for (size_t offset = 0; offset < total_size; offset += PAGE_SIZE_VMM)
  {
    uintptr_t physical = pmm_alloc_page();
    if (!physical)
      return NULL;
    vmm_map(alloc_base + offset, physical, VMM_PRESENT | VMM_WRITE);
  }

  heap_current += total_size;

  block_header_t *block = (block_header_t *)alloc_base;
  block->size = total_size - sizeof(block_header_t);
  block->free = 0;
  block->next = NULL;
  return (void *)((uintptr_t)block + sizeof(block_header_t));
}

static void *kheap_alloc(size_t size)
{
  if (size == 0)
    return NULL;
  size = (size + 15) & ~15ULL;

  block_header_t *current = free_list;

  while (current)
  {
    if (current->free && current->size >= size)
    {
      split_block(current, size);
      current->free = 0;
      return (void *)((uintptr_t)current + sizeof(block_header_t));
    }

    if (!current->next)
      break;
    current = current->next;
  }

  void *new_block = request_more_memory(size);
  if (!new_block)
  {
    kerror("Out of memory");
    return NULL;
  }

  current->next =
      (block_header_t *)((uintptr_t)new_block - sizeof(block_header_t));
  return new_block;
}

static void kheap_free(void *ptr)
{
  if (!ptr)
    return;

  block_header_t *block =
      (block_header_t *)((uintptr_t)ptr - sizeof(block_header_t));
  block->free = 1;

  block_header_t *current = free_list;

  while (current)
  {
    if (current->free && current->next && current->next->free)
    {
      current->size += sizeof(block_header_t) + current->next->size;
      current->next = current->next->next;
    }
    else
    {
      current = current->next;
    }
  }
}

void *kmalloc(size_t size)
{
  return kheap_alloc(size);
}

void *kzalloc(size_t size)
{
  void *ptr = kmalloc(size);
  if (ptr)
    memset(ptr, 0, size);
  return ptr;
}

void *kcalloc(size_t n, size_t size)
{
  if (n && size && n > (SIZE_MAX / size))
    return NULL;
  return kzalloc(n * size);
}

void kfree(void *ptr)
{
  kheap_free(ptr);
}
