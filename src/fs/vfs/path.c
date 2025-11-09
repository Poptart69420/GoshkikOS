#include <fs/vfs/path.h>

static namecache_t namecache; // Name cache

static void make_namecache_key(struct namecache_key *key, struct vnode_t *dvp, const char *name)
{
  key->dvp_ptr = (uintptr_t)dvp;
  strlcpy(key->name, name, sizeof(key->name));
}

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

void namei_init(void)
{
  spinlock_init(&namecache.lock);
  hashtable_init(&namecache.table, 2048);
}
