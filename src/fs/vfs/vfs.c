#include <fs/vfs/vfs.h>

// Static variables
static struct vfs_t *vfs_list_head = NULL;
static struct vfs_t *vfs_list_tail = NULL;
static spinlock_t vfs_lock;

// Lock helpers
static inline void vfs_lock_spin(void)
{
  spinlock_acquire(&vfs_lock);
}

static inline void vfs_unlock_spin(void)
{
  spinlock_release(&vfs_lock);
}

// List helpers
static void vfs_link(struct vfs_t *vfsp) // Spinlock must be locked before calling
{
  if (!vfs_list_head)     // If no list head
    vfs_list_head = vfsp; // List head is the new vfs
  else
    vfs_list_tail->vfs_next = vfsp; // tail next is the new vfs
  vfs_list_tail = vfsp;             // Tail is the new vfs
}

static void vfs_unlink(struct vfs_t *vfsp) // Spinlock must be locked before calling
{
  struct vfs_t **prev = &vfs_list_head;
  struct vfs_t *current = vfs_list_head;

  while (current) // While vfs_list_head
  {
    if (current == vfsp) // If the vfs_list head is the vfs we're looking for
    {
      *prev = current->vfs_next; // Updates previous node to skip over the target vfs node

      if (current == vfs_list_tail)                                // If current is the tail of the list
        vfs_list_tail = (current == vfs_list_head) ? NULL : *prev; // Set to NULL if empty once current is removed, or to previous node

      current->vfs_next = NULL; // Clear the vfs_next of the removed node
      break;
    }

    prev = &current->vfs_next;   // Move tp next
    current = current->vfs_next; // Move to next
  }
}

int vfs_mount_fs(struct vfs_t *vfsp, struct vnode_t *covered, const char *path, int flags)
{
  if (!vfsp || !vfsp->vfs_op || !vfsp->vfs_op->vfs_mount || !path) // If invalid arguments
    return -EINVAL;                                                // Return invalid arguments errno

  vfsp->vfs_vnode_covered = covered; // Set the covering node
  vfsp->vfs_flag = flags;            // Set the flags
  vfsp->vfs_next = NULL;             // set next to NULL

  int ret = vfsp->vfs_op->vfs_mount(vfsp, covered, path, flags); // Call filesystem specific mount function
  if (ret)                                                       // If error is returned
    return ret;                                                  // Pass error code to caller function

  vfs_lock_spin();   // Lock spinlock
  vfs_link(vfsp);    // Link the vfs
  vfs_unlock_spin(); // Unlock spinlock

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

  vfs_unlink(vfsp);  // Unlink the vfs
  vfs_unlock_spin(); // Unlock spinlock

  return 0; // Yay
}

int vfs_root_vnode(struct vfs_t *vfsp, struct vnode_t **root)
{
  if (!vfsp || !vfsp->vfs_op || !vfsp->vfs_op->vfs_root || !root) // If invalid arguments
    return -EINVAL;                                               // Return invalid arguments errno

  return vfsp->vfs_op->vfs_root(vfsp, root); // Call the filesystem specific vfs_root function and pass the return code to the caller function
}

void init_vfs(void)
{
  kprintf("VFS...");
  spinlock_init(&vfs_lock);
  kok();
}
