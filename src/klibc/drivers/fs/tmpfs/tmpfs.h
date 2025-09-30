#ifndef TMPFS_H_
#define TMPFS_H_

#include "../../../list/list.h"
#include "../vfs.h"
#include <stddef.h>

#define TMPFS_FLAGS_FILE 0x01
#define TMPFS_FLAGS_DIR 0x02
#define TMPFS_FLAGS_LINK 0x04

#define IOCTL_TMPFS_CREATE_DEV 0x01
#define IOCTL_TMPFS_SET_DEV_INODE 0x02

struct tmpfs_inode_struct_t;

typedef struct tmpfs_inode_struct_t {
    struct tmpfs_inode_struct_t *parent;
    list_t *entries;
    uint64_t flags;
    size_t buffer_size;
    char *buffer;
    mode_t perm;
    uid_t owner;
    gid_t group_owner;
    size_t link_count;
    size_t open_count;
} tmpfs_inode_t;

typedef struct tmpfs_dirent_struct_t {
    char name[PATH_MAX];
    tmpfs_inode_t *inode;
} tmpfs_dirent_t;

void init_tmpfs(void);

vfs_node_t *new_tmpfs(void);

vfs_node_t *tmpfs_lookup(vfs_node_t *node, const char *name);

ssize_t tmpfs_read(vfs_node_t *node, void *buffer, uint64_t offset,
                   ssize_t count);

ssize_t tmpfs_write(vfs_node_t *node, const void *buffer, uint64_t offset,
                    ssize_t count);

void tmpfs_close(vfs_node_t *node);

int tmpfs_create(vfs_node_t *node, const char *name, mode_t perms,
                 uint64_t flags);

int tmpfs_unlink(vfs_node_t *node, const char *name);

int tmpfs_link(vfs_node_t *, const char *, vfs_node_t *, const char *);

struct dirent_t *tmpfs_readdir(vfs_node_t *node, uint64_t index);

int tmpfs_truncate(vfs_node_t *node, size_t size);

int tmpfs_symlink(vfs_node_t *node, const char *name, const char *target);

ssize_t tmpfs_readlink(vfs_node_t *node, char *buffer, size_t buffer_size);

#endif // TMPFS_H_
