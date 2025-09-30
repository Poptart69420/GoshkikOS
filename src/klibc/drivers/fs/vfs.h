#ifndef VFS_H_
#define VFS_H_

/*
**
** To do: Add attribute support through sys/stat
**
*/

#include "../../include/limits.h"
#include "../../include/types.h"
#include "../../list/list.h"
#include "../../mem/kheap.h"
#include "../../mem/string.h"
#include "../vterm/kerror.h"
#include "../vterm/kok.h"
#include "../vterm/vterm.h"
#include <stddef.h>
#include <stdint.h>

#define VFS_MAX_NODE_NAME_LEN 256
#define VFS_MAX_MOUNT_POINT_NAME_LEN 128
#define VFS_MAX_PATH_LEN 256

#define VFS_FILE 0x01
#define VFS_DIR 0x02
#define VFS_LINK 0x04
#define VFS_DEV 0x08
#define VFS_MOUNT 0x10

#define VFS_READONLY 0x01
#define VFS_WRITEONLY 0x02
#define VFS_READWRITE 0x03
#define VFS_PARENT 0x04
#define VFS_NOFOLOW 0x08

struct vfs_node_struct_t;
struct vfs_mount_point_struct_t;

typedef int mode_t;

struct dirent_t {
    char d_name[VFS_MAX_PATH_LEN];
};

typedef struct vfs_node_struct_t {
    void *private_inode;
    struct vfs_mount_point_struct *mount_point;
    uint64_t flags;
    uint64_t ref_count;
    uint64_t size;

    ssize_t (*read)(struct vfs_node_struct_t *, void *buffer, uint64_t off,
                    ssize_t count);

    ssize_t (*write)(struct vfs_node_struct_t *, const void *buffer,
                     uint64_t off, ssize_t count);

    void (*close)(struct vfs_node_struct_t *);

    struct vfs_node_struct *(*finddir)(struct vfs_node_struct *,
                                       const char *name);

    int (*create)(struct vfs_node_struct_t *, const char *name, mode_t perms,
                  uint64_t);

    int (*unlink)(struct vfs_node_struct_t *, const char *);

    int (*link)(struct vfs_node_struct_t *, const char *,
                struct vfs_node_struct_t *, const char *);

    int (*symlink)(struct vfs_node_struct_t *, const char *, const char *);
    ssize_t (*readlink)(struct vfs_node_struct_t *, char *, size_t);
    struct dirent_t *(*readdir)(struct vfs_node_struct_t *, uint64_t index);
    int (*truncate)(struct vfs_node_struct_t *, size_t);
    int (*ioctl)(struct vfs_node_struct_t *, uint64_t, void *);
    int (*wait_check)(struct vfs_node_struct_t *, short);
    int (*wait)(struct vfs_node_struct_t *, short);
    struct vfs_node_struct_t *(*lookup)(struct vfs_node_struct_t *,
                                        const char *name);

    char name[VFS_MAX_PATH_LEN];
    struct vfs_node_struct_t *parent;
    struct vfs_node_struct_t *sibling;
    struct vfs_node_struct_t *child;
    size_t children_count;

    struct vfs_node_struct_t *linked_node;
} vfs_node_t;

typedef struct vfs_mount_point_struct_t {
    char name[VFS_MAX_MOUNT_POINT_NAME_LEN];
    struct vfs_mount_point_struct_t *prev;
    struct vfs_mount_point_struct_t *next;
    vfs_node_t *root;
    uint64_t flags;
} vfs_mount_point_t;

typedef struct vfs_filesystem_struct_t {
    char name[16];
    int (*mount)(const char *source, const char *target, size_t flags,
                 const void *data);
} vfs_filesystem_t;

extern vfs_node_t *vfs_root;

// Functions

void vfs_register_fs(vfs_filesystem_t *fs);

void init_vfs(void);

int vfs_mount(const char *path, vfs_node_t *local_root);

int vfs_chroot(vfs_node_t *new_root);

vfs_node_t *vfs_open(const char *path, uint64_t flags);

vfs_node_t *vfs_lookup(vfs_node_t *node, const char *name);

ssize_t vfs_read(vfs_node_t *node, void *buffer, uint64_t offset, size_t count);

ssize_t vfs_write(vfs_node_t *node, const void *buffer, uint64_t offset,
                  size_t count);

int vfs_create(const char *path, mode_t perms, uint64_t flags);

int vfs_mkdir(const char *path, mode_t perms);

void vfs_close(vfs_node_t *node);

struct dirent_t *vfs_readdir(vfs_node_t *node, uint64_t index);

ssize_t vfs_readlink(vfs_node_t *node, char *buffer, size_t buffer_size);

int vfs_chroot(vfs_node_t *new_root);

vfs_node_t *vfs_dup(vfs_node_t *node);

int vfs_link(const char *src, const char *dest);

int vfs_symlink(const char *target, const char *linkpath);

vfs_node_t *vfs_openat(vfs_node_t *at, const char *path, uint64_t flags);

#endif // VFS_H_
