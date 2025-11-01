#include <fs/vfs/vfs.h>
#include <fs/vfs/vnode.h>

// Lock helpers
static inline void vfs_lock_vnode_table(void)
{
  spinlock_acquire(&vfs_manager.vnode_table_lock);
}

static inline void vfs_unlock_vnode_table(void)
{
  spinlock_release(&vfs_manager.vnode_table_lock);
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
void vnode_ref(struct vnode_t *vp)
{
  assert(vp);                                            // Assert vp
  __atomic_add_fetch(&vp->v_count, 1, __ATOMIC_RELAXED); // Use atomics for safety. Increase reference count by 1
}

void vnode_unref(struct vnode_t *vp) // Caller must handle locking
{
  assert(vp); // Assert vp

  uint32_t prev_count = __atomic_fetch_sub(&vp->v_count, 1, __ATOMIC_ACQ_REL); // Decrement reference count

  if (prev_count == 0)
  {
    kerror("Vnode reference count already at 0");
    hcf();
  }

  if (prev_count > 1) // If reference count is greater than 1
    return;           // Return (still in use)

  uint32_t prev_flags = __atomic_fetch_or(&vp->v_flag, VFREEING, __ATOMIC_ACQ_REL); // Attempt to aqcuire the freeing flag
  if (prev_flags & VFREEING)                                                        // If flag is already set
    return;                                                                         // Return (finalizing it elsewhere)

  if (vp->v_op && vp->v_op->vn_inactive) // We have control over it
    vp->v_op->vn_inactive(vp);           // Call the filesystem specific function

  __atomic_thread_fence(__ATOMIC_SEQ_CST); // Atomic stuff for safety
  vnode_free(vp);                          // Call vnode free function
}

//
// Vnode management
//
struct vnode_t *vnode_alloc(struct vfs_t *vfsp, enum vtype_t type)
{
  assert(vfs_manager.vnode_cache);                          // If vnode cache doesn't exist panic
  struct vnode_t *vp = slab_alloc(vfs_manager.vnode_cache); // Allocate a slab in the cache

  if (!vp)       // If not allocated (out of mem?)
    return NULL; // Return NULL

  memset(vp, 0, sizeof(*vp));                          // Zero the memory
  vp->v_vfsp = vfsp;                                   // Set the vfs pointer
  vp->v_type = type;                                   // Set the vnode type
  __atomic_store_n(&vp->v_count, 1, __ATOMIC_RELAXED); // Initalize the vnode reference count at 1
  return vp;                                           // Return the pointer of the newly allocated vnode
}

void vnode_free(struct vnode_t *vp)
{
  if (!vp)  // If no vnode pointer
    return; // Return

  if (vp->v_data) // If vnode has data
  {
    kfree(vp->v_data); // Free data
    vp->v_data = NULL; // Set it to NULL
  }

  slab_free(vfs_manager.vnode_cache, vp); // Free the vnode from the cache using the slab free function
}

int vnode_cache_insert(struct vfs_t *vfsp, uint64_t nodeid, struct vnode_t *vp)
{
  if (!vfsp || !vp) // If invalid arguments
    return -EINVAL; // Return invalid arguments error code

  vfs_lock_vnode_table();                                       // Lock vnode table spinlock
  struct vnode_t *existing = NULL;                              // Create temp vnode
  int ret = vnode_cache_lookup_locked(vfsp, nodeid, &existing); // Call lookup, hold return code

  if (ret == 0 && existing) // If return code was sucessful and it exists
  {
    vfs_unlock_vnode_table(); // Unlock vnode table spinlock
    return -EEXIST;           // Return exists error
  }

  ret = vnode_cache_insert_locked(vfsp, nodeid, vp); // Insert the vode into the cache, hold return code
  vfs_unlock_vnode_table();                          // Unlock vnode table spinlock
  return ret;                                        // Return the return code
}

int vnode_cache_lookup(struct vfs_t *vfsp, uint64_t nodeid, struct vnode_t **out_vp)
{
  if (!vfsp || !out_vp) // If invalid arguments
    return -EINVAL;     // Return invalid arguments error code

  vfs_lock_vnode_table();                                 // Lock vnode table spinlock
  struct vnode_t *vp = NULL;                              // Create empty vnode
  int ret = vnode_cache_lookup_locked(vfsp, nodeid, &vp); // Call lookup, hold the teturn code
  if (ret != 0 || !vp)                                    // If return code error or the vnode is NULL
  {
    vfs_unlock_vnode_table(); // Unlock vnode table spinlock
    return -ENOENT;           // Return no entry error code
  }

  vnode_ref(vp);            // Reference the vnode (caller holds it)
  vfs_unlock_vnode_table(); // Unlock vnode table spinlock
  return 0;                 // Yay
}

int vnode_cache_remove(struct vfs_t *vfsp, uint64_t nodeid)
{
  if (!vfsp)        // If invalid argument
    return -EINVAL; // Return invalid argument error code

  vfs_lock_vnode_table();                            // Lock vnode table spinlock
  int ret = vnode_cache_remove_locked(vfsp, nodeid); // Call vnode cache remove, hold return code
  vfs_unlock_vnode_table();                          // Unlock vnode table spinlock
  return ret;                                        // Return the return code
}

int vnode_get(struct vfs_t *vfsp, struct vnode_t **out, uint64_t nodeid)
{
  if (!vfsp || !out) // If invalid arguments
    return -EINVAL;  // Return invalid argument error code

  struct vnode_t *vp = NULL;                       // initalize vp
  int ret = vnode_cache_lookup(vfsp, nodeid, &vp); // Call lookup (search in cache) and store return code

  if (ret == 0) // If it returns found
  {
    *out = vp;  // Set the output pointer to the vp
    return ret; // Yay
  }

  if (!vfsp->vfs_op || !vfsp->vfs_op->vfs_vget) // Functions exist?
    return -ENOSYS;                             // Functions do not exist (filesystem specific)

  uint64_t nodeid_copy = nodeid; // Local nodeid copy

  ret = vfsp->vfs_op->vfs_vget(vfsp, &vp, &nodeid_copy); // Call filesystem specific function

  if (ret)      // If error code returned
    return ret; // Return the passed error code

  if (!vp)       // If no vnode pointer
    return -EIO; // Return I/O error

  vp->v_vfsp = vfsp; // Ensure vnode points to owner vfs_t

  vfs_lock_vnode_table();                                                  // Lock vnode table spinlock
  struct vnode_t *existing = NULL;                                         // Temp to check against a race
  if (vnode_cache_lookup_locked(vfsp, nodeid, &existing) == 0 && existing) // If vnode found
  {
    vnode_ref(existing);      // Reference it (caller function holds it)
    vfs_unlock_vnode_table(); // Unlock vnode table spinlock
    vnode_unref(vp);          // Unreference vp
    *out = existing;          // Set output pointer to the vp
    return 0;                 // Yay 2
  }

  vnode_cache_insert_locked(vfsp, nodeid, vp); // Insert newly created vnode into the global vnode cache
  vfs_unlock_vnode_table();                    // Unlock vnode table spinlock

  *out = vp; // Set output pointer to the vp
  return 0;  // Yay 3
}

void vnode_put(struct vnode_t *vp)
{
  if (!vp)  // If invalid argument
    return; // Return

  vnode_unref(vp); // Unreference the vnode
}

void vfs_purge_vnodes(struct vfs_t *vfsp)
{
  if (!vfsp) // If invalid argument
    return;  // Return

  size_t cap = vfs_manager.vnode_table.capacity; // Capacity = vnode table capacity
  size_t count = 0;                              // Count starts at 0

  vfs_lock_vnode_table(); // Lock vnode table spinlock

  for (size_t i = 0; i < cap; ++i) // Iterate through each bucket index
  {
    for (hash_entry_t *e = vfs_manager.vnode_table.entries[i]; e; e = e->next) // Iterate through each entry in the linked list
    {
      uint64_t *k = (uint64_t *)e->key;            // Get the key
      if ((struct vfs_t *)(uintptr_t)k[0] == vfsp) // Check the first 8 bytes and check if pointer equals the vfsp we are purging
        ++count;                                   // Increment count by 1
    }
  }

  vfs_unlock_vnode_table(); // Unlock vnode table spinlock

  if (count == 0) // If no matching entries were found
    return;       // Return (nothing to purge)

  uint64_t *nodeids = (uint64_t *)kmalloc(sizeof(uint64_t) * count); // Allocate a temp array large enough to hold all node IDs

  if (!nodeids) // If allocation failed
    return;     // Return

  size_t idx = 0;

  vfs_lock_vnode_table(); // Lock vnode table spinlock

  for (size_t i = 0; i < cap; ++i) // Iterate through each bucket index again
  {
    for (hash_entry_t *e = vfs_manager.vnode_table.entries[i]; e; e = e->next) // Interate through each entry in the linked list
    {
      uint64_t *k = (uint64_t *)e->key;            // Get the key
      if ((struct vfs_t *)(uintptr_t)k[0] == vfsp) // Check the first 8 bytes and check if pointer equals the vfsp we are purging
        nodeids[idx++] = k[1];                     // Store the node ID (second 8 bytes)
    }
  }

  vfs_unlock_vnode_table();

  for (size_t j = 0; j < idx; ++j) // Iterate over the collected node IDs
  {
    uint64_t key[2];                         // Prepare key
    create_vnode_key(key, vfsp, nodeids[j]); // Create a identically formatted key to the one used to insert the vnode into the hashtable

    vfs_lock_vnode_table();                                                            // Lock vnode table spinlock
    struct vnode_t *vp = NULL;                                                         // Initalize vnode pointer as NULL
    int get = hashtable_get(&vfs_manager.vnode_table, (void **)&vp, key, sizeof(key)); // Get the value from the key

    if (get == 0 && vp != NULL)
    {
      int rem = hashtable_remove(&vfs_manager.vnode_table, key, sizeof(key)); // Remove the key and value from the table
      vfs_unlock_vnode_table();                                               // Unlock the vnode table spinlock

      if (rem == 0)      // If removed
        vnode_unref(vp); // Unreference it
    }
    else // If not found
    {
      hashtable_remove(&vfs_manager.vnode_table, key, sizeof(key)); // Remove it anyway just in case
      vfs_unlock_vnode_table();                                     // Unlock the vnode table spinlock
    }
  }

  kfree(nodeids); // Free the temp array golding the node IDs
}
