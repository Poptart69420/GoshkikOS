#include <fs/vfs/superblock.h>

static slab_cache_t *superblock_cache = NULL;
static hashtable_t superblock_table;

static inline void make_superblock_key(superblock_hash_key_t *key, const char *name)
{
  key->sb_name_ptr = (uintptr_t)name;
}

static void superblock_cache_init(void)
{
  superblock_cache = slab_cache_create("vfs_superblock", sizeof(superblock_t), alignof(superblock_t), NULL, NULL);
  assert(superblock_cache != NULL);
}

superblock_t *superblock_alloc(const char *name)
{
  if (!superblock_cache)
    superblock_cache_init();

  superblock_t *sb = slab_alloc(superblock_cache);
  if (!sb)
    return NULL;
  memset(sb, 0, sizeof(*sb));
  sb->sb_name = name;
  return sb;
}

void superblock_ref(superblock_t *sb)
{
  assert(sb != NULL);
  sb->ref_count++;
}

void superblock_unref(superblock_t *sb)
{
  if (!sb)
    return;

  assert(sb->ref_count > 0);
  sb->ref_count--;

  if (sb->ref_count == 0)
  {
    if (sb->sb_ops && sb->sb_ops->unmount)
      sb->sb_ops->unmount(sb);
    slab_free(superblock_cache, sb);
  }
}

void superblock_init(superblock_t *sb, const char *name, const struct superblock_operations *ops)
{
  assert(sb != NULL);
  memset(sb, 0, sizeof(*sb));

  sb->sb_name = name;
  sb->sb_ops = ops;
  sb->ref_count = 1;
}

void superblock_cache_destroy(void)
{
  if (superblock_cache)
  {
    slab_destroy(superblock_cache);
    superblock_cache = NULL;
  }
}

int superblock_hash_init(void)
{
  return hashtable_init(&superblock_table, SUPERBLOCK_HASH_SIZE);
}

int superblock_hash_insert(superblock_t *sb)
{
  if (!sb)
    return -EINVAL;

  superblock_hash_key_t key;
  make_superblock_key(&key, sb->sb_name);

  if (hashtable_set(&superblock_table, (void *)sb, &key, sizeof(key)))
    return -ENOMEM;
  return 0;
}

int superblock_hash_remove(superblock_t *sb)
{
  if (!sb)
    return -EINVAL;
  superblock_hash_key_t key;
  make_superblock_key(&key, sb->sb_name);

  return hashtable_remove(&superblock_table, &key, sizeof(key));
}

superblock_t *superblock_hash_get(const char *name)
{
  superblock_hash_key_t key;
  make_superblock_key(&key, name);

  void *value = NULL;
  if (hashtable_get(&superblock_table, &value, &key, sizeof(key)))
    return NULL;

  return (superblock_t *)value;
}

superblock_t *sget(const char *name)
{
  superblock_t *sb = superblock_hash_get(name);
  if (sb)
  {
    superblock_ref(sb);
    return sb;
  }

  sb = superblock_alloc(name);
  if (!sb)
    return NULL;

  if (superblock_hash_insert(sb))
  {
    superblock_unref(sb);
    return NULL;
  }

  return sb;
}

void sfree(superblock_t *sb)
{
  if (!sb)
    return;

  if (sb->ref_count > 1)
  {
    superblock_unref(sb);
    return;
  }

  superblock_hash_remove(sb);
  superblock_unref(sb);
}

void superblock_hash_destroy(void)
{
  hashtable_destroy(&superblock_table);
}
