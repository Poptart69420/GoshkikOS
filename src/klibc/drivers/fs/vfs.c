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
                 size_t count) {
    if (node->write) {
        return node->read(node, (void *)buffer, offset, count);
    } else {
        return -1;
    }

    return -1;
}

ssize_t vfs_write(vfs_node_t *node, const void *buffer, uint64_t offset,
                  size_t count) {
    if (node->write) {
        return node->write(node, (void *)buffer, offset, count);
    } else {
        if (node->flags & VFS_DIR)
            return -1;
    }
    return -1;
}

ssize_t vfs_readlink(vfs_node_t *node, char *buffer, size_t buffer_size) {
    if (node->readlink) {
        return node->readlink(node, buffer, buffer_size);
    } else {
        if (node->flags & VFS_LINK)
            return -1;
    }

    return -1;
}

int vfs_wait_deck(vfs_node_t *node, short type) {
    if (node->wait_check)
        return node->wait_check(node, type);

    return type;
}

int vfs_wait(vfs_node_t *node, short type) {
    if (node->wait)
        return node->wait(node, type);

    return -1;
}

vfs_node_t *vfs_lookup(vfs_node_t *node, const char *name) {
    if ((!strcmp("..", name)) && node->parent)
        return vfs_dup(node->parent);
    if ((!strcmp(".", name)))
        return vfs_dup(node);

    for (vfs_node_t *current = node->child; current;
         current = current->sibling) {
        if (!strcmp(current->name, name)) {
            current->ref_count++;
            return current;
        }
    }

    if (node->lookup) {
        vfs_node_t *child = node->lookup(node, (char *)name);
        if (!child)
            return NULL;
        if (!child->ref_count)
            child->ref_count = 1;

        child->parent = node;
        child->sibling = node->child;
        node->child = child;
        node->children_count++;
        child->child = NULL;
        child->children_count = 0;

        strcpy(child->name, name);
        return child;
    } else {
        return NULL;
    }
}

void vfs_close(vfs_node_t *node) {
    node->ref_count--;

    if (node->ref_count > 0)
        return;

    if (node->flags & VFS_MOUNT || node == root)
        return;

    if (node->children_count > 0)
        return;

    if (node->parent && node->parent != node) {
        if (node->parent->child == node) {
            node->parent->child = node->sibling;
        } else {
            for (vfs_node_t *current = node->parent->child; current;
                 current = current->sibling) {
                if (current->sibling == node) {
                    current->sibling = node->sibling;
                    break;
                }
            }
        }
        node->parent->children_count--;
    }

    vfs_node_t *parent = node->parent;
    if (parent == node)
        parent = NULL;
    if (node->close)
        node->close(node);

    kfree(node);

    if (parent) {
        if (parent->children_count == 0 && parent->ref_count == 0) {
            parent->ref_count++;
            return vfs_close(parent);
        }
    }
}

static const char *vfs_basename(const char *path) {
    const char *base = path + strlen(path) - 1;

    if (*base == '/')
        base--;
    while (*base != '/') {
        base--;
        if (base < path) {
            break;
        }
    }

    base++;
    return base;
}

int vfs_create(const char *path, int perm, uint64_t flags) {
    vfs_node_t *parent = vfs_open(path, VFS_WRITEONLY | VFS_PARENT);
    if (!parent)
        return -1;

    const char *child = vfs_basename(path);
    int ret;

    if (parent->create) {
        ret = parent->create(parent, child, perm, flags);
    } else {
        ret = -1;
    }

    vfs_close(parent);
    return ret;
}

int vfs_mkdir(const char *path, int perm) {
    return vfs_create(path, perm, VFS_DIR);
}

int vfs_unlink(const char *path) {
    vfs_node_t *parent = vfs_open(path, VFS_WRITEONLY | VFS_PARENT);

    if (!parent)
        return -1;

    const char *child = vfs_basename(path);

    int ret;
    if (parent->create) {
        ret = parent->unlink(parent, child);
    } else {
        ret = -1;
    }

    vfs_close(parent);
    return ret;
}

int vfs_symlink(const char *target, const char *linkpath) {
    vfs_node_t *parent = vfs_open(linkpath, VFS_WRITEONLY | VFS_PARENT);
    if (!parent)
        return -1;

    const char *child = vfs_basename(linkpath);
    int ret;
    if (parent->symlink) {
        ret = parent->symlink(parent, child, target);
    } else if (parent->flags & VFS_DIR) {
        ret = -1;
    } else {
        ret = -1;
    }

    vfs_close(parent);
    return ret;
}

int vfs_link(const char *src, const char *dest) {
    vfs_node_t *parent_src = vfs_open(src, VFS_WRITEONLY | VFS_PARENT);
    if (!parent_src)
        return -1;

    vfs_node_t *parent_dest = vfs_open(dest, VFS_WRITEONLY | VFS_PARENT);
    if (!parent_dest)
        return -1;

    const char *child_src = vfs_basename(src);
    const char *child_dest = vfs_basename(dest);

    int ret;
    if (parent_src->link) {
        ret = parent_src->link(parent_src, child_src, parent_dest, child_dest);
    } else {
        ret = -1;
    }

    vfs_close(parent_src);
    vfs_close(parent_dest);
    return ret;
}

struct dirent_t *vfs_readdir(vfs_node_t *node, uint64_t index) {
    if (node->readdir)
        return node->readdir(node, index);
    return NULL;
}

int vfs_truncate(vfs_node_t *node, size_t size) {
    if (node->truncate)
        return node->truncate(node, size);
    if (node->flags & VFS_DIR)
        return -1;
    return -1;
}

int vfs_chmod(vfs_node_t *node, mode_t perm) {
    (void)node;
    (void)perm;

    return -1;
}

int vfs_chown(vfs_node_t *node, uid_t owner, gid_t group_owner) {
    (void)node;
    (void)owner;
    (void)group_owner;

    return -1;
}

// int vfs_setattr(vfs_node_t *node, struct stat *st) {
//     if (!node->setattr)
//         return -1;
//     return node->setattr(node, st);
// }

int vfs_ioctl(vfs_node_t *node, uint64_t request, void *arg) {
    if (node->ioctl)
        return node->ioctl(node, request, arg);
    return -1;
}

vfs_node_t *vfs_dup(vfs_node_t *node) {
    if (!node)
        return NULL;
    node->ref_count++;
    return node;
}

vfs_node_t *vfs_open(const char *path, uint64_t flags) {
    if (path[0] == '/' || path[0] == '\0') {
        return vfs_openat(root, path, flags);
    }

    return NULL;
}

vfs_node_t *vfs_openat(vfs_node_t *at, const char *path, uint64_t flags) {
    if (!(flags & VFS_READONLY) && !(flags & VFS_WRITEONLY)) {
        return NULL;
    }

    char new_path[strlen(path) + 1];
    strcpy(new_path, path);

    int path_depth = 0;
    char last_seperate = 1;
    char *path_array[64];

    for (int i = 0; new_path[i]; ++i) {
        if (new_path[i] == '/') {
            new_path[i] = '\0';
            last_seperate = 1;
            continue;
        }

        if (last_seperate) {
            path_array[path_depth] = &new_path[i];
            path_depth++;
            last_seperate = 0;
        }
    }

    if (flags & VFS_PARENT) {
        if (path_depth < 1) {
            return NULL;
        }
        path_depth--;
    }

    vfs_node_t *current_node = vfs_dup(at);
    int loop_max = SYMLOOP_MAX;

    for (int i = 0; i < path_depth; ++i) {
        if (!current_node)
            return NULL;

        vfs_node_t *next_node = vfs_lookup(current_node, path_array[i]);

        while (next_node) {
            if (next_node->flags & VFS_MOUNT) {
                vfs_node_t *mount_point = next_node;
                next_node = vfs_dup(next_node->linked_node);
                vfs_close(mount_point);
                continue;
            }

            if ((next_node->flags & VFS_LINK) &&
                (!(flags & VFS_NOFOLOW) || i < path_depth - 1)) {
            }
        }
    }
}
