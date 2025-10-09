#ifndef TMPFS_H_
#define TMPFS_H_

#include <arch/x86_64/asm/hcf.h>
#include <fs/vfs/vfs.h>
#include <klibc/assert.h>
#include <klibc/cred/cred.h>
#include <klibc/errno.h>
#include <klibc/kmem/hashtable.h>
#include <klibc/kmem/kheap.h>
#include <klibc/kmem/slab.h>
#include <klibc/kmem/string.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

typedef struct tmpfs_node_t
{
  vnode_t vnode;
  char *name;
  struct tmpfs_node_t *parent;
  struct tmpfs_node_t *next;
  struct tmpfs_node_t *children;
  int type;
  vattr_t attr;
  void *data;
  size_t data_length;
} tmpfs_node_t;

typedef struct tmpfs_fs_t
{
  vfs_t *vfs;
  tmpfs_node_t *root;
} tmpfs_fs_t;

#endif // TMPFS_H_
