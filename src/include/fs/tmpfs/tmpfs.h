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

static int tmpfs_mount(vfs_t **vfs_out, vnode_t *mount_point, vnode_t *backing, void *data);
static int tmpfs_unmount(vfs_t *vfs);
static int tmpfs_root(vfs_t *vfs, vnode_t **root_out);

static int tmpfs_open(vnode_t **node, int flags, cred_t *cred);
static int tmpfs_close(vnode_t *node, int flags, cred_t *cred);
static int tmpfs_read(vnode_t *node, size_t size, uintmax_t offset, int flags, size_t *readc, cred_t *cred, void *buffer);
static int tmpfs_write(vnode_t *node, size_t size, uintmax_t offset, int flags, size_t *writec, cred_t *cred, void *buffer);
static int tmpfs_lookup(vnode_t *node, char *name, vnode_t **result, cred_t *cred);
static int tmpfs_create(vnode_t *parent, char *name, vattr_t *attr, int type, vnode_t **result, cred_t *cred);
static int tmpfs_getattr(vnode_t *node, vattr_t *attr, cred_t *cred);
static int tmpfs_setattr(vnode_t *node, vattr_t *attr, int attrs, cred_t *cred);
static int tmpfs_unlink(vnode_t *node, vnode_t *child, char *name, cred_t *cred);
static int tmpfs_symlink(vnode_t *parent, char *name, vattr_t *attr, char *path, cred_t *cred);
static int tmpfs_readlink(vnode_t *parent, char **link, cred_t *cred);
static int tmpfs_inactive(vnode_t *node);

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
  char *symlink_target;
} tmpfs_node_t;

typedef struct tmpfs_fs_t
{
  vfs_t *vfs;
  tmpfs_node_t *root;
} tmpfs_fs_t;

extern vfs_ops_t tmpfs_vfs_ops;
extern vops_t tmpfs_vops;

#endif // TMPFS_H_
