#include "vfs.h"
#include "../../../kernel.h"

vfs_node_t *root;

list_t *fs_types;

void init_vfs(void) {
    root = NULL;
    fs_types = new_list();
}

void vfs_register_fs(vfs_filesystem_t *fs) { list_append(fs_types, fs); }

void vfs_deregister_fs(vfs_filesystem_t *fs) { list_remove(fs_types, fs); }

int vfs_mount(const char *name, vfs_node_t *local_root) {
    vfs_node_t *mount_point = vfs_open(name, VFS_READWRITE);
    if (!mount_point) {
        if (vfs_mkdir(name, 0777)) {
            return -1;
        }

        mount_point = vfs_open(name, VFS_READWRITE);
        if (!mount_point) {
            return -1;
        }
    }

    if (mount_point->flags & VFS_MOUNT) {
        return -1;
    }

    mount_point->linked_node = local_root;
    local_root->ref_count++;
    mount_point->ref_count++;

    mount_point->flags |= VFS_MOUNT;

    local_root->parent = mount_point->parent;
    return 0;
}

int vfs_unmount(const char *path) {
    vfs_node_t *parent = vfs_open(path, VFS_PARENT);
    if (!parent)
        return -1;

    const char *child = path + strlen(path - 1);
    if (*child == '/')
        child--;
    while (*child != '/') {
        child--;
        if (child < path)
            break;
    }

    vfs_node_t *mount_point = vfs_lookup(parent, path);
    vfs_close(parent);
    if (!(mount_point->flags & VFS_MOUNT))
        return -1;

    vfs_node_t *local_root = mount_point->linked_node;
    mount_point->flags &= ~VFS_MOUNT;

    vfs_close(local_root);
    vfs_close(mount_point);
    return 0;
}

ssize_t vfs_read(vfs_node_t *node, void *buffer, uint64_t offset,
                 size_t count) {}
