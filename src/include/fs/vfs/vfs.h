#ifndef VFS_H_
#define VFS_H_

#include <arch/x86_64/asm/hcf.h>
#include <klibc/assert.h>
#include <klibc/cred/cred.h>
#include <klibc/errno.h>
#include <klibc/kmem/hashtable.h>
#include <klibc/kmem/kheap.h>
#include <klibc/kmem/slab.h>
#include <klibc/kmem/string.h>
#include <klibc/types.h>
#include <scheduling/spinlock.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/types.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

// VFS flags
#define VFS_RONLY 0x01   // Read-only
#define VFS_REMOUNT 0x02 // Remount existing filesystem
#define VFS_NOEXEC 0x04  // No execution of binaries
#define VFS_NOSUID 0x08  // Ignore set user ID and set group ID
#define VFS_SYNC 0x10    // All writes are synchronous

#define VROOT 0x01   // Vnode is the root of its filesystem
#define VTEXT 0x02   // Vnode is a pure text file
#define VSYSTEM 0x04 // Vnode represents a system internal object

// Forward declerations
struct vnode_t;
struct vnodeops_t;
struct vfs_t;
struct vfsops_t;
extern struct vfs_manager_t vfs_manager;

enum vtype_t
{
  VNON, // No type
  VREG, // Regular file
  VDIR, // Directory
  VBLK, // Block device
  VCHR, // Character device
  VLNK, // Symbolic link
  VSCK, // Socket
  VBAD  // Bad vnode
};

typedef struct v_attr_t
{
  enum vtype_t va_type;    // Vnode type
  umode_t va_mode;         // File permissions and type bits (such as 0755)
  uid_t va_uid;            // Owner user ID
  gid_t va_gid;            // Owner group ID
  uint64_t va_fsid;        // Filesystem ID
  uint64_t va_nodeid;      // Inode number
  uint32_t va_nlink;       // Number of hard links
  uint64_t va_size;        // File size in bytes
  uint64_t va_blocksize;   // Preferred I/O block size
  struct timeval va_atime; // Last access time
  struct timeval va_mtime; // Last modification time
  struct timeval va_ctime; // Last status change time
  dev_t va_rdev;           // Device ID
  uint64_t va_blocks;      // Number of 512-byte blocks allocated
} v_attr_t;

struct vfs_t
{
  struct vfs_t *vfs_next;            // Next in the linked list
  struct vfsops_t *vfs_op;           // VFS operations
  struct vnode_t *vfs_vnode_covered; // Vnode being covered
  uint32_t vfs_flag;                 // Mount flags
  uint64_t vfs_bsize;                // Native block size
  void *vfs_data;                    // Private filesystem data
};

struct vnode_t
{
  uint32_t v_flag;             // Vnode flags
  uint32_t v_count;            // Reference count
  uint32_t v_shlockc;          // Shared lock count
  uint32_t v_exlockc;          // Exclusive lock count
  struct vfs_t *v_vfs_mounted; // Covering VFS
  struct vnodeops_t *v_op;     // Vnode operations

  union
  {
    void *v_socket; // Socket
    void *v_stream; // Stream
  } v_un;

  struct vfs_t *v_vfsp; // Parent VFS
  enum vtype_t v_type;  // Vnode type
  void *v_data;         // Private file data
};

typedef struct vfs_manager_t
{
  hashtable_t mount_table; // Key: mount path | Value: struct vfs_t *
  hashtable_t vnode_table; // Key: inode/FID  | Value: struct vnode_t*
  spinlock_t lock;         // Spinlock lock
  struct slab_cache_t *vfs_cache;
  struct slab_cache_t *vnode_cache;
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

struct vnodeops_t
{
  int (*vn_open)(struct vnode_t *, int, int);                                       // Opens a file or device
  int (*vn_close)(struct vnode_t *, int);                                           // Closes an open vnode, maybe flushing or decrementing reference counts
  int (*vn_rdwr)(struct vnode_t *, void *, size_t, off64_t, int);                   // Reads or writes file data (read/write option supplied by caller)
  int (*vn_ioctl)(struct vnode_t *, int, void *);                                   // Device of filesystem specific control operations
  int (*vn_select)(struct vnode_t *, int);                                          // Checks I/O readiness
  int (*vn_getattr)(struct vnode_t *, v_attr_t *);                                  // Retrieves metadata
  int (*vn_setattr)(struct vnode_t *, v_attr_t *);                                  // Sets metadata
  int (*vn_access)(struct vnode_t *, int);                                          // Checks if current user has permissions
  int (*vn_lookup)(struct vnode_t *, const char *, struct vnode_t **);              // Returns the target vnode (directory) within a directory vnode
  int (*vn_create)(struct vnode_t *, const char *, int, int, struct vnode_t **);    // Creates a file under a directory node
  int (*vn_remove)(struct vnode_t *, const char *);                                 // Removes a file from a directory vnode
  int (*vn_link)(struct vnode_t *, struct vnode_t *, const char *);                 // Creates a hard link from a file to another directory entry
  int (*vn_rename)(struct vnode_t *, const char *, struct vnode_t *, const char *); // Moves or renames a file or directory
  int (*vn_mkdir)(struct vnode_t *, const char *, int, struct vnode_t **);          // Creates a directory
  int (*vn_rmdir)(struct vnode_t *, const char *);                                  // Removes a directory
  int (*vn_readdir)(struct vnode_t *, void *, size_t, off64_t *);                   // Reads directory entries into a user buffer
  int (*vn_symlink)(struct vnode_t *, const char *, const char *);                  // Creates a symbolic link
  int (*vn_readlink)(struct vnode_t *, char *, size_t);                             // Reads the contents of the target symbolic link
  int (*vn_fsync)(struct vnode_t *);                                                // Flushes file data and metadata to stable storage
  int (*vn_inactive)(struct vnode_t *);                                             // Called when vnode is no longer in use (for cleanup or caching)
  int (*vn_bmap)(struct vnode_t *, off64_t, struct vnode_t **, off64_t *);          // Translates logical file offset to a physical block
  int (*vn_strategy)(void *);                                                       // Handle block I/O request
  int (*vn_bread)(struct vnode_t *, off64_t, void **);                              // Reads a specific block from disk into memory
  int (*vn_brelse)(void *);                                                         // Releases a previously read block buffer
};

// VFS functions
void init_vfs(void);
int vfs_mount_fs(struct vfs_t *vfsp, struct vnode_t *covered, const char *path, int flags);
int vfs_unmount_fs(struct vfs_t *vfsp, int flags);
int vfs_root_vnode(struct vfs_t *vfsp, struct vnode_t **root);
struct vfs_t *vfs_from_vnode(struct vnode_t *vp);
struct vfs_t *vfs_from_path(const char *path);
void vfs_for_each(void (*call_back)(struct vfs_t *));

//
// Vnode functions
//

struct vnode_t *vnode_alloc(struct vfs_t *vfsp, enum vtype_t type);
void vnode_free(struct vnode_t *vp);
int vnode_cache_insert(struct vfs_t *vfsp, uint64_t nodeid, struct vnode_t *vp);
int vnode_cache_lookup(struct vfs_t *vfsp, uint64_t nodeid, struct vnode_t **out_vp);
int vnode_cache_remove(struct vfs_t *vfsp, uint64_t nodeid);
int vnode_get(struct vfs_t *vfsp, struct vnode_t **out, uint64_t nodeid);
void vnode_put(struct vnode_t *vp);
void vfs_purge_vnodes(struct vfs_t *vfsp);

#endif // VFS_H_
