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
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

#define V_FLAGS_ROOT 1
#define V_TYPE_REGULAR 0
#define V_TYPE_DIR 1
#define V_TYPE_CHDEV 2
#define V_TYPE_BLKDEV 3
#define V_TYPE_FIFO 4
#define V_TYPE_LINK 5
#define V_TYPE_SOCKET 6

#define V_FFLAGS_READ 1
#define V_FFLAGS_WRITE 2
#define V_FFLAGS_NONBLOCKING 4
#define V_FFLAGS_SHARED 8
#define V_FFLAGS_EXEC 16
#define V_FFLAGS_NOCTTY 32
#define V_FFLAGS_NOCACHE 64

#define PATHNAME_MAX 512
#define MAXLINKDEPTH 64

typedef struct
{
  int type;
  mode_t mode;
  uid_t uid;
  gid_t gid;
  int fs_id;
  ino_t inode;
  int nlinks;
  size_t size;
  size_t fs_block_size;
  size_t blocks_used;
} vattr_t;

typedef struct vfs_t
{
  struct vfs_t *next;
  struct vfs_ops_t *ops;
  struct vnode_t *node_covered;
  struct vnode_t *root;
  int flags;
} vfs_t;

typedef struct vnode_t
{
  struct vops_t *ops;
  int ref_count;
  int flags;
  int type;
  vfs_t *vfs;
  vfs_t *vfs_mounted;
} vnode_t;

typedef struct vfs_ops_t
{
  int (*mount)(vfs_t **vfs, vnode_t *mount_point, vnode_t *backing, void *data);
  int (*unmount)(vfs_t *vfs);
  int (*sync)(vfs_t *vfs);
  int (*root)(vfs_t *vfs, vnode_t **root);
} vfs_ops_t;

typedef struct vops_t
{
  int (*open)(vnode_t **node, int flags, cred_t *cred);
  int (*close)(vnode_t *node, int flags, cred_t *cred);
  int (*read)(vnode_t *node, size_t size, uintmax_t offset, int flags, size_t *readc, cred_t *cred);
  int (*write)(vnode_t *node, size_t size, uintmax_t offset, int flags, size_t *writec, cred_t *cred);
  int (*lookup)(vnode_t *node, char *name, vnode_t **result, cred_t *cred);
  int (*create)(vnode_t *parent, char *name, vattr_t *attr, int type, vnode_t **result, cred_t *cred);
  int (*getattr)(vnode_t *node, vattr_t *attr, cred_t *cred);
  int (*setattr)(vnode_t *node, vattr_t *attr, int attrs, cred_t *cred);
  int (*access)(vnode_t *node, mode_t mode, cred_t *cred);
  int (*unlink)(vnode_t *node, vnode_t *child, char *name, cred_t *cred);
  int (*link)(vnode_t *node, vnode_t *dir, char *name, cred_t *cred);
  int (*symlink)(vnode_t *parent, char *name, vattr_t *attr, char *path, cred_t *cred);
  int (*readlink)(vnode_t *parent, char **link, cred_t *cred);
  int (*inactive)(vnode_t *node);
  int (*mmap)(vnode_t *node, void *addr, uintmax_t offset, int flags, cred_t *cred);
  int (*munmap)(vnode_t *node, void *addr, uintmax_t offset, int flags, cred_t *cred);
  int (*isatty)(vnode_t *node);
  int (*maxseek)(vnode_t *node, size_t *max);
  int (*resize)(vnode_t *node, size_t newsize, cred_t *cred);
  int (*rename)(vnode_t *source_dir, vnode_t *source, char *old_name, vnode_t *target_dir, vnode_t *target, char *new_name, int flags);
  int (*sync)(vnode_t *node);
} vops_t;

extern vnode_t *vfs_root;

vnode_t *vnode_alloc(vops_t *ops, int type, vfs_t *vfs);
void vnode_hold(vnode_t *vnode);
void vnode_put(vnode_t *vnode);
int vfs_register(vfs_t **out, vfs_ops_t *ops);
int vfs_mount(vfs_t **out, vfs_ops_t *ops, vnode_t *mount_point, vnode_t *backing, void *data);
int vfs_unmount(vfs_t *vfs);
int vfs_get_root(vfs_t *vfs, vnode_t **root_out);
int vfs_resolve_absolute(const char *path, vnode_t **res, cred_t *cred);
int vnode_lookup(vnode_t *dir, const char *name, vnode_t **result, cred_t *cred);
int vfs_open(vnode_t **node, int flags, cred_t *cred);
int vfs_close(vnode_t *node, int flags, cred_t *cred);
int vfs_write(vnode_t *node, size_t size, uintmax_t offset, int flags, size_t *writec, cred_t *cred);
int vfs_create(vnode_t *parent, char *name, vattr_t *attr, int type, vnode_t **result, cred_t *cred);
int vfs_getattr(vnode_t *node, vattr_t *attr, cred_t *cred);
int vfs_setattr(vnode_t *node, vattr_t *attr, int attrs, cred_t *cred);
int vfs_set_root(vnode_t *root);
int init_vfs(void);

#endif // VFS_H_
