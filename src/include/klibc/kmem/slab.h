#ifndef SLAB_H_
#define SLAB_H_

#include <arch/x86_64/mem/pmm/pmm.h>
#include <arch/x86_64/mem/vmm/vmm.h>
#include <klibc/kmem/kheap.h>
#include <stddef.h>
#include <stdint.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

#define PAGE_SIZE_SLAB PAGE_SIZE_VMM
#define SLAB_MAGIC 0x534C4142

struct slab_cache_t;
struct slab_t;

typedef struct slab_t
{
  struct slab_t *next;
  struct slab_cache_t *cache;
  uint32_t free_count;
  uint32_t total_objects;
  uint32_t magic;
  void *free_list;
} slab_t;

typedef struct slab_cache_t
{
  const char *name;
  size_t object_size;
  size_t align;
  size_t objects_per_slab;
  slab_t *partial;
  slab_t *full;
  slab_t *empty;
  void (*ctor)(void *);
  void (*dtor)(void *);
} slab_cache_t;

void *slab_alloc(struct slab_cache_t *cache_in);

void slab_free(struct slab_cache_t *cache_in, void *object);

void slab_destroy(struct slab_cache_t *cache);

struct slab_cache_t *slab_cache_create(const char *name, size_t object_size,
                                       size_t align, void (*ctor)(void *),
                                       void (*dtor)(void *));

#endif // SLAB_H_
