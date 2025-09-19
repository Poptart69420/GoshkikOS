#ifndef VFS_H_
#define VFS_H_

#include "vfs_structs.h"
#include "../vterm/vterm.h"
#include "pparse.h"
#include "../../mem/string.h"

#define MAX_FILESYSTEMS 8
#define MAX_MOUNTS 16

void init_vfs(void);
int vfs_register_filesystem(filesystem_t *fs);
int vfs_mount(const char *fs_name, void *mount_data, const char *mount_path);
vfs_node_t *vfs_root(void);
vfs_node_t *vfs_lookup(const char *path);
size_t vfs_read(vfs_node_t *node, size_t offset, size_t size, void *buffer);
size_t vfs_write(vfs_node_t *node, size_t offset, size_t size, const void *buffer);
int vfs_open(vfs_node_t *node);
int vfs_close(vfs_node_t *node);
int vfs_readdir(vfs_node_t *node, size_t index, vfs_dirent_t *dirent);
vfs_node_t *vfs_finddir(vfs_node_t *node, const char *name);
vfs_node_t *vfs_create_file(const char *path, const void *content, size_t size);
vfs_node_t *vfs_create_dir(const char *path);

#endif // VFS_H_
