#ifndef VFS_H_
#define VFS_H_

#include <klibc/kmem/kheap.h>
#include <klibc/kmem/slab.h>
#include <klibc/kmem/string.h>
#include <stddef.h>
#include <stdint.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

#define VFS_REG 0
#define VFS_DIR 1

#define VFS_FLAG_READ 1
#define VFS_FLAG_write 2

typedef uint64_t vfs_ino_t;
typedef uint64_t vfs_mode_t;
typedef uint64_t vfs_off_t;
typedef uint64_t vfs_flags_t;

typedef uint32_t uid_t;
typedef uint32_t gid_t;

struct vfs_inode_t;
struct vfs_dirent_t;
struct vfs_file_t;

typedef struct vfs_file_t_ops {
    int (*open)(struct vfs_file_t *f);
    int (*read)(struct vfs_file_t *f, void *buf, size_t len, vfs_off_t off);
    int (*write)(struct vfs_file_t *f, const void *buf, size_t len,
                 vfs_off_t off);
    int (*truncate)(struct vfs_file_t *f, vfs_off_t size);
    int (*close)(struct vfs_file_t *f);
} vfs_file_ops_t;

typedef struct vfs_inode_ops {
    int (*lookup)(struct vfs_inode_t *dir, struct vfs_dirent_t *entry);
    int (*create)(struct vfs_inode_t *dir, struct vfs_dirent_t *entry,
                  struct vfs_inode_t **out);
    int (*unlink)(struct vfs_inode_t *dir, struct vfs_dirent_t *entry);
    int (*mkdir)(struct vfs_inode_t *dir, struct vfs_dirent_t *entry);
    int (*rmdir)(struct vfs_inode_t *dir, struct vfs_dirent_t *entry);
    int (*stat)(struct vfs_inode_t *inode, void *statbuf);
} vfs_inode_ops_t;

typedef struct {
    int type; // File/directory/etc
    vfs_mode_t mode;
    vfs_flags_t flags;
    uid_t uid;
    gid_t gid;
    int fsid;      // Filesystem id
    vfs_ino_t ino; // Inode number
    vfs_off_t size;
} vfs_attr_t;

typedef struct vfs_inode_t {
    int type;
    vfs_flags_t flags;
    size_t ref_count;
    void *private;
    const vfs_inode_ops_t *ops;
} vfs_inode_t;

typedef struct vfs_file_t {
    vfs_inode_t *inode;
    vfs_off_t offset;
    size_t ref_count;
    void *private;
    const vfs_file_ops_t *ops;
    vfs_attr_t attributes;
} vfs_file_t;

typedef struct vfs_dirent_t {
    char *name; // NULL terminated
    vfs_inode_t *inode;
    size_t ref_count;
    void *private; // Filesystem specific data
    vfs_attr_t attributes;

    struct vfs_dirent_t *parent;
    struct vfs_dirent_t *child;
    struct vfs_dirent_t *next_sibling;
} vfs_dirent_t;

typedef struct vfs_filesystem_t {
    const char *fs_name; // NULL terminated
    vfs_dirent_t *root;
    struct vfs_filesystem_t *next;
} vfs_filesystem_t;

int init_vfs(void);

#endif // VFS_H_
