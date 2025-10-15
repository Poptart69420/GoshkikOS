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
