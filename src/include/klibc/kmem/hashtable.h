#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <klibc/kmem/kheap.h>
#include <klibc/kmem/slab.h>
#include <klibc/kmem/string.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

#define FNV1PRIME 0x100000001b3ull
#define FNV1OFFSET 0xcbf29ce484222325ull

typedef struct hash_entry_t
{
  struct hash_entry_t *next;
  struct hash_entry_t *prev;
  uintmax_t hash;
  size_t key_size;
  void *key;
  void *value;
} hash_entry_t;

typedef struct
{
  size_t entry_count;
  size_t capacity;
  hash_entry_t **entries;
} hashtable_t;

int hashtable_init(hashtable_t *table, size_t size);
int hashtable_set(hashtable_t *table, void *value, void *key, size_t key_size);
int hashtable_get(hashtable_t *table, void **value, void *key, size_t key_size);
int hashtable_remove(hashtable_t *table, void *key, size_t key_size);
int hashtable_destroy(hashtable_t *table);

#endif // HASHTABLE_H_
