#ifndef DENTRY_H_
#define DENTRY_H_

#include <arch/x86_64/asm/hcf.h>
#include <fs/vfs/vfs.h>
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

// VFS DENTRY
struct vfs_dentry
{
  inode_t *vn_inode;          // Associated inode
  vfs_dentry_t *vn_parent;    // Parent node
  vfs_dentry_t **vn_children; // Array of child nodes
  size_t vn_child_count;      // Number of children
  vfs_dentry_t *vn_next;      // Sibling / hash bucket list
  char vn_name[PATHNAME_MAX]; // Node name
  unsigned int vn_ref_count;  // Reference count
};

#endif // DENTRY_H_
