#ifndef VFS_H_
#define VFS_H_

#include <arch/x86_64/asm/hcf.h>
#include <errno.h>
#include <fs/vfs/vnode.h>
#include <klibc/assert.h>
#include <klibc/cred/cred.h>
#include <klibc/kmem/hashtable.h>
#include <klibc/kmem/kheap.h>
#include <klibc/kmem/slab.h>
#include <klibc/kmem/string.h>
#include <scheduling/spinlock.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

// Limits
#define VFS_MOUNT_PATH_MAX 256
#define NAME_MAX 255

// VFS flags
#define VFS_RONLY 0x01   // Read-only
#define VFS_REMOUNT 0x02 // Remount existing filesystem
#define VFS_NOEXEC 0x04  // No execution of binaries
#define VFS_NOSUID 0x08  // Ignore set user ID and set group ID
#define VFS_SYNC 0x10    // All writes are synchronous

// Forward declerations
struct vnode_t;
struct vnodeops_t;
struct vfs_t;
struct vfsops_t;
extern struct vfs_manager_t vfs_manager;

struct vfs_t
{
  struct vfs_t *vfs_next;                  // Next in the linked list
  struct vfsops_t *vfs_op;                 // VFS operations
  struct vnode_t *vfs_vnode_covered;       // Vnode being covered
  uint32_t vfs_flag;                       // Mount flags
  uint64_t vfs_bsize;                      // Native block size
  void *vfs_data;                          // Private filesystem data
  char vfs_mount_path[VFS_MOUNT_PATH_MAX]; // Canonical mount path
};

typedef struct vfs_manager_t
{
  hashtable_t mount_table;          // Key: mount path | Value: struct vfs_t *
  hashtable_t vnode_table;          // Key: inode/FID  | Value: struct vnode_t*
  spinlock_t vnode_table_lock;      // Vnode table lock
  spinlock_t mount_table_lock;      // Mount table lock
  struct slab_cache_t *vfs_cache;   // VFS cache
  struct slab_cache_t *vnode_cache; // Vnode cache
} vfs_manager_t;

struct vfsops_t
{
  int (*vfs_mount)(struct vfs_t *, struct vnode_t *, const char *, int); // Mount filesystem to global namespace
  int (*vfs_unmount)(struct vfs_t *, int);                               // Unmount filesystem from the global namespace
  int (*vfs_root)(struct vfs_t *, struct vnode_t **);                    // Returns the root vnode of the filesystem
  int (*vfs_statfs)(struct vfs_t *, void *);                             // Returns filesystem statistics
  int (*vfs_sync)(struct vfs_t *, int);                                  // Flushes all modified buffers and metadata to disk
  int (*vfs_fid)(struct vnode_t *, void *);                              // Generates a unique file ID for a vnode
  int (*vfs_vget)(struct vfs_t *, struct vnode_t **, void *);            // Returns a vnode of a given file (using fileID or inode number)
};

//
// VFS functions
//
void init_vfs(void);
int vfs_mount_fs(struct vfs_t *vfsp, struct vnode_t *covered, const char *path, int flags);
int vfs_unmount_fs(struct vfs_t *vfsp, int flags);
int vfs_root_vnode(struct vfs_t *vfsp, struct vnode_t **root);
struct vfs_t *vfs_from_vnode(struct vnode_t *vp);
struct vfs_t *vfs_from_path(const char *path);
void vfs_for_each(void (*call_back)(struct vfs_t *));
struct vfs_t *vfs_find_mounted_on_vnode(struct vnode_t *vp);

#endif // VFS_H_
