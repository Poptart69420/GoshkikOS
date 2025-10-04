#include <klibc/kmem/hashtable.h>

static inline uint64_t fnv1ahash(void *buffer, size_t size) {
    uint8_t *ptr = buffer;
    uint8_t *top = ptr + size;
    uint64_t h = FNV1OFFSET;

    while (ptr < top) {
        h ^= *ptr++;
        h *= FNV1PRIME;
    }

    return h;
}

static slab_cache_t *hash_entry_cache;

static hash_entry_t *get_entry(hashtable_t *table, void *key, size_t key_size,
                               uintmax_t hash) {
    uintmax_t table_offset = hash % table->capacity;

    hash_entry_t *entry = table->entries[table_offset];

    while (entry) {
        if (entry->key_size == key_size && entry->hash == hash &&
            memcmp(entry->key, key, key_size) == 0)
            break;
        entry = entry->next;
    }

    return entry;
}

int hashtable_set(hashtable_t *table, void *value, void *key, size_t key_size) {
    uintmax_t hash = fnv1ahash(key, key_size);
    hash_entry_t *entry = get_entry(table, key, key_size, hash);

    if (entry) {
        entry->value = value;
        return 0;
    } else {
        uintmax_t table_offset = hash % table->capacity;
        entry = slab_alloc(hash_entry_cache);
        if (entry == NULL)
            return -1;
        entry->key = kmalloc(key_size);
        if (entry->key == NULL) {
            slab_free(hash_entry_cache, entry);
            return -1;
        }

        memcpy(entry->key, key, key_size);
        entry->prev = NULL;
        entry->next = table->entries[table_offset];
        if (entry->next)
            entry->next->prev = entry;
        entry->value = value;
        entry->key_size = key_size;
        entry->hash = hash;
        table->entries[table_offset] = entry;
        ++table->entry_count;
        return 0;
    }

    return -1;
}

int hashtable_get(hashtable_t *table, void **value, void *key,
                  size_t key_size) {
    uintmax_t hash = fnv1ahash(key, key_size);
    hash_entry_t *entry = get_entry(table, key, key_size, hash);

    if (entry == NULL)
        return -1;

    *value = entry->value;
    return 0;
}

int hashtable_remove(hashtable_t *table, void *key, size_t key_size) {
    uintmax_t hash = fnv1ahash(key, key_size);
    uintmax_t table_offset = hash % table->capacity;

    hash_entry_t *entry = get_entry(table, key, key_size, hash);

    if (entry == NULL)
        return -1;

    if (entry->prev) {
        entry->prev->next = entry->next;
    } else {
        table->entries[table_offset] = entry->next;
    }

    if (entry->next)
        entry->next->prev = entry->prev;

    kfree(entry->key);
    slab_free(hash_entry_cache, entry);
    --table->entry_count;

    return 0;
}

int hashtable_destroy(hashtable_t *table) {
    for (size_t i = 0; i < table->capacity; ++i) {
        hash_entry_t *entry = table->entries[i];
        while (entry) {
            hash_entry_t *next = entry->next;
            kfree(entry->key);
            slab_free(hash_entry_cache, entry);
            entry = next;
        }
    }
    kfree(table->entries);
    table->entries = NULL;
    table->entry_count = 0;

    return 0;
}

int hashtable_init(hashtable_t *table, size_t size) {
    if (hash_entry_cache == NULL) {
        hash_entry_cache = slab_cache_create(
            "hashtable_entry", sizeof(hash_entry_t), 0, NULL, NULL);
        if (hash_entry_cache == NULL)
            return -1;
    }

    table->capacity = size;
    table->entries = kmalloc(size * sizeof(hash_entry_t *));
    if (table->entries == NULL)
        return -1;

    memset(table->entries, 0, size * sizeof(hash_entry_t *));
    table->entry_count = 0;

    return 0;
}
