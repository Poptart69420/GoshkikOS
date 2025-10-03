#include <klibc/kmem/slab.h>

static inline size_t align_up(size_t v, size_t a) {
    return (v + (a - 1)) & ~(a - 1);
}

static inline slab_t *slab_from_obj(void *object) {
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

    size_t header_size = align_up(sizeof(slab_t), sizeof(void *));
    size_t first_object_offset = align_up(header_size, cache->object_size);
    size_t usable = PAGE_SIZE_SLAB - first_object_offset;
    cache->objects_per_slab = usable / cache->object_size;

    if (cache->objects_per_slab == 0)
        return NULL;

    return cache;
}

static void *slab_pop_object(slab_t *slab) {
    if (!slab || !slab->free_list)
        return NULL;
    void *object = slab->free_list;
    slab->free_list = *(void **)object;
    slav->free_count--;
    return object;
}

static void slab_push_object(slab_t *slab, void *object) {
    *(void **)object = slab->free_list;
    slab->free_list = object;
    slab->free_count++;
}
