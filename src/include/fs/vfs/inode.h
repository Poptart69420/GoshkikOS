#ifndef INODE_H_
#define INODE_H_

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

// Core inode structure
struct inode
{
  umode_t i_mode;                      // File mode (permissions + type)
  unsigned short i_opflags;            // Optional operation flags
  cred_t i_cred;                       // Owner credentials
  unsigned int i_flags;                // General inode flags
  const struct inode_operations *i_op; // Inode operations
  const struct file_operations *i_fop; // File operations
  superblock_t *i_sb;                  // Parent superblock
  unsigned long i_ino;                 // Unique inode number
  loff_t i_size;                       // File size in bytes
  unsigned int ref_count;              // Simple reference count
};

// Inode hash key
typedef struct inode_hash_key
{
  uintptr_t sb_ptr;
  unsigned long ino;
} inode_hash_key_t;

// Inode-level operations (create, mkdir, unlink, lookup)
struct inode_operations
{
  int (*create)(inode_t *dir, inode_t **out, const char *name);
  int (*mkdir)(inode_t *dir, const char *name);
  int (*unlink)(inode_t *dir, const char *name);
  int (*lookup)(inode_t *dir, const char *name, inode_t **out);
  int (*destroy_inode)(inode_t *inode);
};

// Inode functions
void inode_init(inode_t *inode, superblock_t *sb, unsigned long ino, umode_t mode, const struct inode_operations *i_op, const struct file_operations *f_op);
inode_t *inode_alloc(struct superblock *sb);
void inode_ref(inode_t *inode);
void inode_unref(inode_t *inode);
void inode_set_size(inode_t *inode, loff_t size);
void inode_reset(inode_t *inode);
void inode_cache_destroy(void);

#endif
