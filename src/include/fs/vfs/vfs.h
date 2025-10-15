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
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

#ifndef alignof
#define alignof(type) __alignof__(type)
#endif

#define V_FLAGS_ROOT 1

// File types
#define V_TYPE_REGULAR 0
#define V_TYPE_DIR 1
#define V_TYPE_CHDEV 2
#define V_TYPE_BLKDEV 3
#define V_TYPE_FIFO 4
#define V_TYPE_LINK 5
#define V_TYPE_SOCKET 6

// File open flags
#define V_FFLAGS_READ 0x01
#define V_FFLAGS_WRITE 0x02
#define V_FFLAGS_NONBLOCKING 0x04
#define V_FFLAGS_SHARED 0x08
#define V_FFLAGS_EXEC 0x10
#define V_FFLAGS_NOCTTY 0x20
#define V_FFLAGS_NOCACHE 0x40

#define PATHNAME_MAX 512
#define MAXLINKDEPTH 64
#define INODE_HASH_SIZE 256

// Struct defs
typedef struct inode inode_t;
typedef struct vfs_node vfs_node_t;
typedef struct superblock superblock_t;

// File-level operations (read/write/ioctl, etc.)
struct file_operations
{
  ssize_t (*read)(inode_t *inode, char *buf, size_t len, loff_t *offset);
  ssize_t (*write)(inode_t *inode, const char *buf, size_t len, loff_t *offset);
};

// VFS node
struct vfs_node
{
  inode_t *vn_inode;          // Associated inode
  vfs_node_t *vn_parent;      // Parent node
  vfs_node_t **vn_children;   // Array of child nodes
  size_t vn_child_count;      // Number of children
  vfs_node_t *vn_next;        // Sibling / hash bucket list
  char vn_name[PATHNAME_MAX]; // Node name
};

#endif // VFS_H_
