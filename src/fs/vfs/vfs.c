#include <fs/vfs/vfs.h>

// Global VFS manager
vfs_manager_t vfs_manager;

// Lock helpers
static inline void vfs_lock_spin(void)
{
  spinlock_acquire(&vfs_manager.lock);
}

static inline void vfs_unlock_spin(void)
{
  spinlock_release(&vfs_manager.lock);
}

// Vnode hashtable helpers
static inline void create_vnode_key(void *out_key, struct vfs_t *vfsp, uint64_t nodeid)
{
  uint64_t *k = (uint64_t *)out_key; // Out key is treated as a two element uin64_t array
  k[0] = (uint64_t)(uintptr_t)vfsp;  // First 8 bytes of the out key is the vfsp pointer value
  k[1] = nodeid;                     // Last 8 bytes of the out key is the nodeid
}

static int vnode_cache_insert_locked(struct vfs_t *vfsp, uint64_t nodeid, struct vnode_t *vp) // Caller must handle locking
{
  uint64_t key[2];                                                      // 16 byte key (uint64_t / 2)
  create_vnode_key(key, vfsp, nodeid);                                  // Call create vnode key function
  return hashtable_set(&vfs_manager.vnode_table, vp, key, sizeof(key)); // Call the hashtable set function and pass the return code to the caller function
}

static int vnode_cache_lookup_locked(struct vfs_t *vfsp, uint64_t nodeid, struct vnode_t **out_vp) // Caller must handle locking
{
  uint64_t key[2];                                                                      // 16 byte key (uint64_t / 2)
  create_vnode_key(key, vfsp, nodeid);                                                  // Call create vnode key function
  int ret = hashtable_get(&vfs_manager.vnode_table, (void **)out_vp, key, sizeof(key)); // Does hashtable_get return something
  return ret;                                                                           // Did hashtable_get return a value?
}

static int vnode_cache_remove_locked(struct vfs_t *vfsp, uint64_t nodeid)
{
  uint64_t key[2];                                                     // 16 byte key (uint64_t / 2)
  create_vnode_key(key, vfsp, nodeid);                                 // Call create vnode key function
  return hashtable_remove(&vfs_manager.vnode_table, key, sizeof(key)); // Call hashtable remove function and pass return code to the caller function
}

// Vnode reference count helpers
static inline void vnode_ref(struct vnode_t *vp)
{
  assert(vp);                                            // Assert vp
  __atomic_add_fetch(&vp->v_count, 1, __ATOMIC_RELAXED); // Use atomics for safety. Increase reference count by 1
}

static inline void vnode_unref(struct vnode_t *vp)
{
  assert(vp); // Assert vp

  if (__atomic_sub_fetch(&vp->v_count, 1, __ATOMIC_RELAXED) == 0) // Use atomics for safety. Decrease reference count by 1, and check if it's 0
  {
    if (vp->v_op && vp->v_op->vn_inactive)  // If vp has a vn_inactive function
      vp->v_op->vn_inactive(vp);            // Call vp inactive function
    slab_free(vfs_manager.vnode_cache, vp); // Free vp from vnode cache
  }
}

int vfs_mount_fs(struct vfs_t *vfsp, struct vnode_t *covered, const char *path, int flags)
{
  if (!vfsp || !vfsp->vfs_op || !vfsp->vfs_op->vfs_mount || !path) // If invalid arguments
    return -EINVAL;                                                // Return invalid arguments errno

  vfsp->vfs_vnode_covered = covered; // Set the covering node
  vfsp->vfs_flag = flags;            // Set the flags
  vfsp->vfs_next = NULL;             // set next to NULL

  if (covered)
    vnode_ref(covered);

  int ret = vfsp->vfs_op->vfs_mount(vfsp, covered, path, flags); // Call filesystem specific mount function
  if (ret)                                                       // If error is returned
  {
    if (covered)            // If covered
      vnode_unref(covered); // Decrease covered vnode reference count

    return ret; // Pass error code to caller function
  }
  vfs_lock_spin();                                                               // Lock spinlock
  hashtable_set(&vfs_manager.mount_table, vfsp, (void *)path, strlen(path) + 1); // Add to mount hashtable. Key: path | Value: vfsp
  vfs_unlock_spin();                                                             // Unlock spinlock

  return 0; // Yay
}

int vfs_unmount_fs(struct vfs_t *vfsp, int flags)
{
  if (!vfsp || !vfsp->vfs_op || !vfsp->vfs_op->vfs_unmount) // If invalid arguments
    return -EINVAL;                                         // Return invalid arguments errno

  vfs_lock_spin(); // Lock spinlock

  int ret = vfsp->vfs_op->vfs_unmount(vfsp, flags); // Call filesystem specific unmount function
  if (ret)                                          // If error is returned
  {
    vfs_unlock_spin(); // Unlock spinlock
    return ret;        // Pass error code to caller function
  }

  hashtable_remove(&vfs_manager.mount_table, (void *)vfsp->vfs_vnode_covered->v_data, strlen((char *)vfsp->vfs_vnode_covered->v_data) + 1); // Remove from mount hashtable

  if (vfsp->vfs_vnode_covered)            // If covered node exists
    vnode_unref(vfsp->vfs_vnode_covered); // decrease the reference count by 1

  vfs_unlock_spin();                      // Unlock spinlock
  slab_free(vfs_manager.vfs_cache, vfsp); // Free the vfsp from the vfs cache

  return 0; // Yay
}

int vfs_root_vnode(struct vfs_t *vfsp, struct vnode_t **root)
{
  if (!vfsp || !vfsp->vfs_op || !vfsp->vfs_op->vfs_root || !root) // If invalid arguments
    return -EINVAL;                                               // Return invalid arguments errno

  int ret = vfsp->vfs_op->vfs_root(vfsp, root); // Call the filesystem specific vfs_root function and store the return code

  if (ret == 0 && *root) // If no error code passed
    vnode_ref(*root);    // Increase root vnode reference count by 1 (caller references it)

  return ret; // Return passed code
}

struct vfs_t *vfs_from_vnode(struct vnode_t *vp)
{
  if (!vp)       // If no vnode pointer provided
    return NULL; // Return NULL

  return vp->v_vfs_mounted; // Direct pointed stored in vnode
}

struct vfs_t *vfs_from_path(const char *path)
{
  if (!path)     // If no path given
    return NULL; // Return NULL

  vfs_lock_spin();                                                                        // Lock spinlock
  struct vfs_t *vfsp = NULL;                                                              // Declare vfsp
  hashtable_get(&vfs_manager.mount_table, (void *)&vfsp, (void *)path, strlen(path) + 1); // Retrieve from hastable (path is the key)
  vfs_unlock_spin();                                                                      // Unlock spinlock
  return vfsp;                                                                            // Return vfsp (if not in hashtable this should return NULL)
}

void vfs_for_each(void (*call_back)(struct vfs_t *))
{
  if (!call_back) // If no call back function
    return;       // Return, because we just won't do shit otherwise

  vfs_lock_spin(); // Lock spinlock

  for (size_t i = 0; i < vfs_manager.mount_table.capacity; ++i) // Loop through table
  {
    hash_entry_t *entry = vfs_manager.mount_table.entries[i]; // Advance entry
    while (entry)                                             // While entry != NULL
    {
      call_back((struct vfs_t *)entry->value); // Call the provided call back function
      entry = entry->next;                     // Advance to entries next entry
    }
  }

  vfs_unlock_spin(); // Unlock spinlock
}

void init_vfs(void)
{
  kprintf("VFS...");
  spinlock_init(&vfs_manager.lock); // Lock the vfs_manager spinlock

  hashtable_init(&vfs_manager.mount_table, 128); // Init the mount hashtable with a size of 128
  hashtable_init(&vfs_manager.vnode_table, 256); // Init the vnode hashtable with a size of 256

  vfs_manager.vfs_cache = slab_cache_create("vfs_cache", sizeof(struct vfs_t), 0, NULL, NULL);       // Create the vfs cache (slab cache)
  vfs_manager.vnode_cache = slab_cache_create("vnode_cache", sizeof(struct vnode_t), 0, NULL, NULL); // Create the vnode cache (slab cache)

  kok();
}
