#ifndef SUPERBLOCK_H_
#define SUPERBLOCK_H_

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

// Superblock-level operations (mount/unmount)
struct superblock_operations
{
  int (*mount)(superblock_t *sb);
  int (*unmount)(superblock_t *sb);
};

// Superblock hash key
typedef struct
{
  uintptr_t sb_name_ptr;
} superblock_hash_key_t;

// Superblock (filesystem instance)
struct superblock
{
  const char *sb_name;                        // Filesystem name
  void *sb_data;                              // Filesystem-specific data
  unsigned int ref_count;                     // Superblock reference count
  vfs_dentry_t *sb_root;                      // Root node of the filesystem
  const struct superblock_operations *sb_ops; // Superblock operations
};

#endif
