#include <klibc/kmem/slab.h>

static inline size_t align_up(size_t v, size_t a) {
  return (v + (a - 1)) & ~(a - 1);
}

static inline slab_t *slab_from_object(void *object) {
  uintptr_t base = (uintptr_t)object & ~(PAGE_SIZE_SLAB - 1);
  return (slab_t *)base;
}

static slab_t *slab_init(slab_cache_t *cache) {
  uintptr_t physical = pmm_alloc_page();
  if (!physical)
    return NULL;

  uintptr_t virtual_address = physical;
  vmm_map(virtual_address, physical, VMM_PRESENT | VMM_WRITE);

  slab_t *slab = (slab_t *)virtual_address;
  memset(slab, 0, PAGE_SIZE_SLAB);

  size_t header_size = align_up(sizeof(slab_t), sizeof(void *));
  size_t first_object_offset = header_size;
  first_object_offset = align_up(first_object_offset, cache->align);

  size_t usable = PAGE_SIZE_SLAB - first_object_offset;
  size_t objects = usable / cache->object_size;

  if (objects == 0) {
    return NULL;
  }

  slab->cache = cache;
  slab->total_objects = (uint32_t)objects;
  slab->free_count = slab->total_objects;
  slab->magic = SLAB_MAGIC;

  uint8_t *object = (uint8_t *)virtual_address + first_object_offset;
  void *previous = NULL;

  for (size_t i = 0; i < objects; ++i) {
    void *slot = (void *)object;
    *(void **)slot = previous;
    previous = slot;
    object += cache->object_size;
  }

  slab->free_list = previous;
  return slab;
}

struct slab_cache_t *slab_cache_create(const char *name, size_t object_size,
                                       size_t align, void (*ctor)(void *),
                                       void (*dtor)(void *)) {
  if (object_size == 0)
    return NULL;

  if (align < sizeof(void *))
    align = sizeof(void *);

  slab_cache_t *cache = kmalloc(sizeof(*cache));
  if (!cache)
    return NULL;

  cache->name = name;
  cache->align = align;
  cache->ctor = ctor;
  cache->dtor = dtor;
  cache->partial = NULL;
  cache->full = NULL;
  cache->empty = NULL;
  cache->object_size = align_up(object_size, cache->align);

  size_t header_size = align_up(sizeof(slab_t), sizeof(void *));
  size_t first_object_offset = align_up(header_size, cache->align);
  size_t usable = PAGE_SIZE_SLAB - first_object_offset;
  cache->objects_per_slab = usable / cache->object_size;

  if (cache->objects_per_slab == 0) {
    kfree(cache);
    return NULL;
  }

  return cache;
}

static void *slab_pop_object(slab_t *slab) {
  if (!slab || !slab->free_list)
    return NULL;
  void *object = slab->free_list;
  slab->free_list = *(void **)object;
  slab->free_count--;
  return object;
}

static void slab_push_object(slab_t *slab, void *object) {
  *(void **)object = slab->free_list;
  slab->free_list = object;
  slab->free_count++;
}

void *slab_alloc(struct slab_cache_t *cache_in) {
  slab_cache_t *cache = (slab_cache_t *)cache_in;
  if (!cache) {
    return NULL;
  }

  slab_t *slab = cache->partial;
  if (!slab)
    slab = cache->empty;
  if (!slab) {
    slab = slab_init(cache);
    if (!slab) {
      return kmalloc(cache->object_size);
    }

    slab->next = cache->partial;
    cache->partial = slab;
  }

  void *object = slab_pop_object(slab);
  if (!object) {
    return NULL;
  }

  if (slab->free_count == 0) {
    slab_t **partial_pointer = &cache->partial;
    while (*partial_pointer && *partial_pointer != slab)
      partial_pointer = &(*partial_pointer)->next;
    if (*partial_pointer)
      *partial_pointer = slab->next;
    slab->next = cache->full;
    cache->full = slab;
  }

  if (cache->ctor)
    cache->ctor(object);

  return object;
}

void slab_free(struct slab_cache_t *cache_in, void *object) {
  if (!cache_in || !object) {
    return;
  }

  slab_cache_t *cache = (slab_cache_t *)cache_in;
  slab_t *slab = slab_from_object(object);
  if (slab->magic != SLAB_MAGIC) {
    kfree(object);
    return;
  }

  if (cache->dtor)
    cache->dtor(object);

  int was_full = (slab->free_count == 0);
  slab_push_object(slab, object);

  if (was_full) {
    slab_t **partial_pointer = &cache->full;
    while (*partial_pointer && *partial_pointer != slab)
      partial_pointer = &(*partial_pointer)->next;
    if (*partial_pointer) {
      *partial_pointer = slab->next;
      slab->next = cache->partial;
      cache->partial = slab;
    }

    return;
  }

  if (slab->free_count == slab->total_objects) {
    slab_t **partial_pointer = &cache->partial;
    while (*partial_pointer && *partial_pointer != slab)
      partial_pointer = &(*partial_pointer)->next;
    if (*partial_pointer)
      *partial_pointer = slab->next;

    uintptr_t slab_virtual_address = (uintptr_t)slab;
    uintptr_t slab_physical_address =
        virtual_to_physical((void *)slab_virtual_address);
    vmm_unmap(slab_virtual_address);
    pmm_free_page(slab_physical_address);
  }
}

void slab_destroy(struct slab_cache_t *cache) {
  if (!cache) {
    return;
  }

  slab_t *slab;

  slab = cache->partial;
  while (slab) {
    slab_t *next = slab->next;
    uintptr_t slab_virtual_address = (uintptr_t)slab;
    uintptr_t slab_physical_address =
        virtual_to_physical((void *)slab_virtual_address);
    vmm_unmap(slab_virtual_address);
    pmm_free_page(slab_physical_address);
    slab = next;
  }

  slab = cache->empty;
  while (slab) {
    slab_t *next = slab->next;
    uintptr_t slab_virtual_address = (uintptr_t)slab;
    uintptr_t slab_physical_address =
        virtual_to_physical((void *)slab_virtual_address);
    vmm_unmap(slab_virtual_address);
    pmm_free_page(slab_physical_address);
    slab = next;
  }

  kfree(cache);
}
