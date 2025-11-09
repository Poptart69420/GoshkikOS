#include <fs/vfs/vfs.h>

// Global VFS manager
vfs_manager_t vfs_manager;

// Lock helpers
static inline void vfs_lock_mount_table(void)
{
  spinlock_acquire(&vfs_manager.mount_table_lock);
}

static inline void vfs_unlock_mount_table(void)
{
  spinlock_release(&vfs_manager.mount_table_lock);
}

//
// VFS management
//
void vfs_for_each(void (*call_back)(struct vfs_t *))
{
  if (!call_back)
    return; // Return, because we just won't do shit otherwise

  vfs_lock_mount_table();

  for (size_t i = 0; i < vfs_manager.mount_table.capacity; ++i) // Loop through table
  {
    hash_entry_t *entry = vfs_manager.mount_table.entries[i]; // Advance entry
    while (entry)                                             // While entry != NULL
    {
      call_back((struct vfs_t *)entry->value); // Call the provided call back function
      entry = entry->next;                     // Advance to entries next entry
    }
  }

  vfs_unlock_mount_table();
}

int vfs_root_vnode(struct vfs_t *vfsp, struct vnode_t **root)
{
  if (!vfsp || !vfsp->vfs_op || !vfsp->vfs_op->vfs_root || !root)
    return -EINVAL; // Return invalid arguments errno

  int ret = vfsp->vfs_op->vfs_root(vfsp, root); // Call the filesystem specific vfs_root function and store the return code

  if (ret == 0 && *root) // If no error code passed
    vnode_ref(*root);    // Increase root vnode reference count by 1 (caller references it)

  return ret; // Return passed code
}

struct vfs_t *vfs_from_vnode(struct vnode_t *vp)
{
  if (!vp)       // If no vnode pointer provided
    return NULL; // Return NULL

  return vp->v_vfsp; // Direct pointed stored in vnode
}

struct vfs_t *vfs_from_path(const char *path)
{
  if (!path)     // If no path given
    return NULL; // Return NULL

  vfs_lock_mount_table();
  struct vfs_t *vfsp = NULL;                                                              // Declare vfsp
  hashtable_get(&vfs_manager.mount_table, (void *)&vfsp, (void *)path, strlen(path) + 1); // Retrieve from hastable (path is the key)
  vfs_unlock_mount_table();
  return vfsp; // Return vfsp (if not in hashtable this should return NULL)
}

struct vfs_t *vfs_find_mounted_on_vnode(struct vnode_t *vp)
{
  if (!vp)
    return NULL;

  spinlock_acquire(&vfs_manager.mount_table_lock);

  for (size_t i = 0; i < vfs_manager.mount_table.capacity; ++i) // Loop through each slot
  {
    for (hash_entry_t *entry = vfs_manager.mount_table.entries[i]; entry; entry = entry->next) // Loop through each entry
    {
      struct vfs_t *vfsp = (struct vfs_t *)entry->value; // Get the entry value
      if (vfsp && vfsp->vfs_vnode_covered == vp)         // If it is what we're looking for
      {
        spinlock_release(&vfs_manager.mount_table_lock);
        return vfsp; // Yay
      }
    }
  }

  spinlock_release(&vfs_manager.mount_table_lock);
  return NULL; // Not found
}

int vfs_mount_fs(struct vfs_t *vfsp, struct vnode_t *covered, const char *path, int flags)
{
  if (!vfsp || !vfsp->vfs_op || !vfsp->vfs_op->vfs_mount || !path)
    return -EINVAL; // Return invalid arguments errno

  vfsp->vfs_vnode_covered = covered; // Set the covering node
  vfsp->vfs_flag = flags;            // Set the flags
  vfsp->vfs_next = NULL;             // set next to NULL

  if (covered)
    vnode_ref(covered);

  int ret = vfsp->vfs_op->vfs_mount(vfsp, covered, path, flags); // Call filesystem specific mount function
  if (ret)
  {
    if (covered)
      vnode_unref(covered); // Decrease covered vnode reference count

    return ret; // Pass error code to caller function
  }
  strlcpy(vfsp->vfs_mount_path, path, sizeof(vfsp->vfs_mount_path)); // Copy the mount path
  vfs_lock_mount_table();
  hashtable_set(&vfs_manager.mount_table, vfsp, vfsp->vfs_mount_path, strlen(vfsp->vfs_mount_path) + 1); // Add to mount hashtable. Key: path | Value: vfsp
  vfs_unlock_mount_table();

  return 0; // Yay
}

int vfs_unmount_fs(struct vfs_t *vfsp, int flags)
{
  if (!vfsp || !vfsp->vfs_op || !vfsp->vfs_op->vfs_unmount)
    return -EINVAL; // Return invalid arguments errno

  vfs_lock_mount_table();

  int ret = vfsp->vfs_op->vfs_unmount(vfsp, flags); // Call filesystem specific unmount function
  if (ret)                                          // If error is returned
  {
    vfs_unlock_mount_table();
    return ret; // Pass error code to caller function
  }

  hashtable_remove(&vfs_manager.mount_table, vfsp->vfs_mount_path, strlen(vfsp->vfs_mount_path) + 1); // Remove from mount hashtable

  if (vfsp->vfs_vnode_covered)            // If covered node exists
    vnode_unref(vfsp->vfs_vnode_covered); // decrease the reference count by 1

  vfs_unlock_mount_table();
  vfs_purge_vnodes(vfsp); // Remove nodes from this filesystem

  slab_free(vfs_manager.vfs_cache, vfsp);

  return 0; // Yay
}

void init_vfs(void)
{
  kprintf("VFS...");
  spinlock_init(&vfs_manager.vnode_table_lock); // Initialize the vnode table spinlock
  spinlock_init(&vfs_manager.mount_table_lock); // Initialize the mount table spinlock

  hashtable_init(&vfs_manager.mount_table, 128); // Init the mount hashtable with a size of 128
  hashtable_init(&vfs_manager.vnode_table, 256); // Init the vnode hashtable with a size of 256

  vfs_manager.vfs_cache = slab_cache_create("vfs_cache", sizeof(struct vfs_t), 0, NULL, NULL);       // Create the vfs cache (slab cache)
  vfs_manager.vnode_cache = slab_cache_create("vnode_cache", sizeof(struct vnode_t), 0, NULL, NULL); // Create the vnode cache (slab cache)

  kok();
}
