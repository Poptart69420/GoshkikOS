#include <fs/vfs/path.h>

static namecache_t namecache; // Name cache

static void make_namecache_key(struct namecache_key *key, struct vnode_t *dvp, const char *name)
{
  key->dvp_ptr = (uintptr_t)dvp;
  strlcpy(key->name, name, sizeof(key->name));
}

// Return root vnode for "/" mount if present (referenced)
static struct vnode_t *namei_get_root_vnode(void)
{
  struct vfs_t *vfsp = vfs_from_path("/");
  struct vnode_t *root = NULL;

  if (vfsp || vfsp->vfs_op || vfsp->vfs_op->vfs_root)
  {
    if (vfsp->vfs_op->vfs_root(vfsp, &root) == 0 && root)
    {
      vnode_ref(root);
      return root;
    }
  }

  return NULL;
}

// Returns 0 and a vnode in out_vp is sucessful (referenced)
int namecache_lookup(struct vnode_t *dvp, const char *name, struct vnode_t **out_vp)
{
  if (!dvp || !name || !out_vp)
    return -EINVAL;

  struct namecache_key key;
  make_namecache_key(&key, dvp, name); // Get the key

  spinlock_acquire(&namecache.lock);
  struct vnode_t *vp = NULL;

  int status = hashtable_get(&namecache.table, (void **)&vp, &key, sizeof(key)); // Try to get the vnode pointer (hold return code)

  if (status != 0 || !vp)
  {
    spinlock_release(&namecache.lock);
    return -ENOENT; // Not found
  }

  vnode_ref(vp); // Reference it (caller holds the reference)
  spinlock_release(&namecache.lock);

  *out_vp = vp; // Set the pointer from the caller
  return 0;     // Yay
}

// Insert a single name cache entry (referenced)
int namecache_enter(struct vnode_t *dvp, const char *name, struct vnode_t *vp)
{
  if (!dvp || !name || !vp)
    return -EINVAL;

  struct namecache_key key;
  make_namecache_key(&key, dvp, name); // Get the key

  vnode_ref(vp); // Reference it (caller holds the reference)

  spinlock_acquire(&namecache.lock);
  int status = hashtable_set(&namecache.table, vp, &key, sizeof(key)); // Try to get the vnode pointer (hold return code)
  spinlock_release(&namecache.lock);

  if (status)
  {
    vnode_unref(vp); // Unreference if there is a error code returned
    return status;   // Error code
  }

  return 0; // Yay
}

// Remove a single name cache entry
void namecache_remove(struct vnode_t *dvp, const char *name)
{
  if (!dvp || !name)
    return;

  struct namecache_key key;
  make_namecache_key(&key, dvp, name);

  spinlock_acquire(&namecache.lock);
  struct vnode_t *vp = NULL;
  int status = hashtable_get(&namecache.table, (void **)&vp, &key, sizeof(key));

  if (status == 0 && vp)
    vnode_unref(vp); // Unreference the vnode pointer if status is a success

  hashtable_remove(&namecache.table, &key, sizeof(key)); // Remove from hashtable even if status is a failure (defensive)
  spinlock_release(&namecache.lock);
}

void namei_init(void)
{
  spinlock_init(&namecache.lock);
  hashtable_init(&namecache.table, 2048);
}
