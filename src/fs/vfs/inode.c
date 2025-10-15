#include <fs/vfs/inode.h>

static slab_cache_t *inode_cache = NULL;
static hashtable_t inode_table;

static inline void make_inode_key(inode_hash_key_t *key, superblock_t *sb, unsigned long ino)
{
  key->sb_ptr = (uintptr_t)sb;
  key->ino = ino;
}

static void inode_cache_init(void)
{
  inode_cache = slab_cache_create("vfs_inode", sizeof(inode_t), alignof(inode_t), NULL, NULL);
  assert(inode_cache != NULL);
}

inode_t *inode_alloc(superblock_t *sb)
{
  if (!inode_cache)
    inode_cache_init();

  inode_t *inode = slab_alloc(inode_cache);
  if (!inode)
    return NULL;
  memset(inode, 0, sizeof(*inode));
  inode->i_sb = sb;
  inode->ref_count = 1;
  return inode;
}

void inode_ref(inode_t *inode)
{
  assert(inode != NULL);
  inode->ref_count++;
}

static void inode_cleanup(inode_t *inode)
{
  if (inode->i_op && inode->i_op->destroy_inode)
    inode->i_op->destroy_inode(inode);
}

void inode_unref(inode_t *inode)
{
  if (!inode)
    return;

  assert(inode->ref_count > 0);
  inode->ref_count--;

  if (inode->ref_count == 0)
  {
    inode_cleanup(inode);
    slab_free(inode_cache, inode);
  }
}

void inode_init(inode_t *inode, superblock_t *sb, unsigned long ino, umode_t mode, const struct inode_operations *i_op, const struct file_operations *f_op)
{
  assert(inode != NULL);

  inode->i_sb = sb;
  inode->i_ino = ino;
  inode->i_mode = mode;
  inode->i_op = i_op;
  inode->i_fop = f_op;
  inode->i_size = 0;
  inode->i_flags = 0;
  inode->i_opflags = 0;
}

void inode_set_size(inode_t *inode, loff_t size)
{
  assert(inode != NULL);
  inode->i_size = size;
}

void inode_reset(inode_t *inode)
{
  assert(inode != NULL);
  memset(inode, 0, sizeof(*inode));
}

void inode_cache_destroy(void)
{
  if (inode_cache)
  {
    slab_destroy(inode_cache);
    inode_cache = NULL;
  }
}

int inode_hash_init(void)
{
  return hashtable_init(&inode_table, INODE_HASH_SIZE);
}

int inode_hash_insert(inode_t *inode)
{
  if (!inode)
    return -EINVAL;

  inode_hash_key_t key;
  make_inode_key(&key, inode->i_sb, inode->i_ino);

  if (hashtable_set(&inode_table, (void *)inode, &key, sizeof(key)))
    return -ENOMEM;

  return 0;
}

int inode_hash_remove(inode_t *inode)
{
  if (!inode)
    return -EINVAL;

  inode_hash_key_t key;
  make_inode_key(&key, inode->i_sb, inode->i_ino);

  return hashtable_remove(&inode_table, &key, sizeof(key));
}

inode_t *inode_hash_get(superblock_t *sb, unsigned long ino)
{
  inode_hash_key_t key;
  make_inode_key(&key, sb, ino);

  void *value = NULL;
  if (hashtable_get(&inode_table, &value, &key, sizeof(key)))
    return NULL;

  return (inode_t *)value;
}

inode_t *iget(superblock_t *sb, unsigned long ino)
{
  inode_t *inode = inode_hash_get(sb, ino);
  if (inode)
  {
    inode_ref(inode);
    return inode;
  }

  inode = inode_alloc(sb);
  if (!inode)
    return NULL;

  inode->i_ino = ino;

  if (inode_hash_insert(inode))
  {
    inode_unref(inode);
    return NULL;
  }

  return inode;
}

void iput(inode_t *inode)
{
  if (!inode)
    return;

  if (inode->ref_count > 1)
  {
    inode_unref(inode);
    return;
  }

  inode_hash_remove(inode);
  inode_unref(inode);
}

void inode_hash_destroy(void)
{
  hashtable_destroy(&inode_table);
}
