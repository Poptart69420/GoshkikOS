#ifndef VFS_H_
#define VFS_H_

#include "../../include/types.h"
#include "../../list/list.h"
#include "../../mem/kheap.h"
#include "../../mem/string.h"
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

struct vfs_node_struct;
struct vfs_mount_point_struct;

typedef int mode_t;

struct dirent {
    char d_name[VFS_MAX_PATH_LEN];
};

typedef struct vfs_node_struct_t {
    void *private_inode;
    struct vfs_mount_point_struct *mount_point;
    uint64_t flags;
    uint64_t ref_count;
    uint64_t size;
    int64_t (*read)(struct vfs_node_struct *, void *buffer, uint64_t off,
                    size_t count);
    int64_t (*write)(struct vfs_node_struct *, void *buffer, uint64_t off,
                     size_t count);
    void (*close)(struct vfs_node_struct *);
    struct vfs_node_struct *(*finddir)(struct vfs_node_struct *,
                                       const char *name);
    int (*create)(struct vfs_node_struct *, const char *name, mode_t perms,
                  uint64_t);
    int (*unlink)(struct vfs_node_struct *, const char *);
    struct dirent *(*readdir)(struct vfs_node_struct *, uint64_t index);
    int (*truncate)(struct vfs_node_struct *, size_t);
    int (*ioctl)(struct vfs_node_struct *, uint64_t, void *);

    char name[VFS_MAX_PATH_LEN];
    struct vfs_node_struct *parent;
    struct vfs_node_struct *sibling;
    struct vfs_node_struct *child;
    size_t children_count;

    struct vfs_node_struct_t *linked_node;
} vfs_node_t;

typedef struct vfs_mount_point_struct {
    char name[VFS_MAX_MOUNT_POINT_NAME_LEN];
    struct vfs_mount_point_struct *prev;
    struct vfs_mount_point_struct *next;
    vfs_node_t *root;
    uint64_t flags;
} vfs_mount_point_t;

typedef struct vfs_filesystem_struct {
    char name[16];
    int (*mount)(const char *source, const char *target, unsigned long flags,
                 const void *data);
} vfs_filesystem_t;

// Functions

void init_vfs(void);
int vfs_mount(const char *path, vfs_node_t *local_root);
int vfs_chroot(vfs_node_t *new_root);
vfs_node_t *vfs_open(const char *path, uint64_t flags);
vfs_node_t *vfs_lookup(vfs_node_t *node, const char *name);
ssize_t vfs_read(vfs_node_t *node, void *buffer, uint64_t offset, size_t count);
ssize_t vfs_write(vfs_node_t *node, const void *buffer, uint64_t offset,
                  size_t count);
int vfs_create(const char *path, int perm, uint64_t flags);
int vfs_mkdir(const char *path, int perm);
void vfs_close(vfs_node_t *node);
#endif // VFS_H_
