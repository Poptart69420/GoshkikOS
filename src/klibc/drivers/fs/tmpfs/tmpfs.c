#include "tmpfs.h"

static char *strdup(const char *str) {
    if (!str)
        return NULL;
    size_t str_size = strlen(str) + 1;
    char *new_str = kmalloc(str_size);
    if (!new_str)
        return NULL;
    return strcpy(new_str, str);
}

static tmpfs_inode_t *new_inode(uint64_t flags) {
    tmpfs_inode_t *inode = kmalloc(sizeof(tmpfs_inode_t));
    memset(inode, 0, sizeof(tmpfs_inode_t));
    inode->buffer_size = 0;
    inode->buffer = NULL;
    inode->flags = flags;
    inode->parent = NULL;
    inode->entries = new_list();
    inode->link_count = 1;

    return inode;
}

static void free_inode(tmpfs_inode_t *inode) {
    kfree(inode->buffer);
    free_list(inode->entries);
    kfree(inode);
}

static vfs_node_t *inode_to_node(tmpfs_inode_t *inode) {
    vfs_node_t *node = kmalloc(sizeof(vfs_node_t));
    memset(node, 0, sizeof(vfs_node_t));
    node->private_inode = (void *)inode;
    node->flags = 0;

    if (inode->flags & TMPFS_FLAGS_DIR) {
        node->lookup = tmpfs_lookup;
        node->readdir = tmpfs_readdir;
        node->create = tmpfs_create;
        node->unlink = tmpfs_unlink;
        node->link = tmpfs_link;
        node->symlink = tmpfs_symlink;
        node->flags |= VFS_DIR;
    }

    if (inode->flags & TMPFS_FLAGS_FILE) {
        node->read = tmpfs_read;
        node->write = tmpfs_write;
        node->truncate = tmpfs_truncate;
        node->flags |= VFS_FILE;
    }

    if (inode->flags & TMPFS_FLAGS_LINK) {
        node->readlink = tmpfs_readlink;
        node->flags |= VFS_LINK;
    }

    node->size = inode->buffer_size;

    node->close = tmpfs_close;
    inode->open_count++;
    return node;
}

static int tmpfs_exist(tmpfs_inode_t *inode, const char *name) {
    foreach (node, inode->entries) {
        const tmpfs_dirent_t *entry = node->value;
        if (!strcmp(name, entry->name))
            return 1;
    }
    return 0;
}

int tmpfs_mount(const char *src, const char *dest, size_t flags,
                const void *data) {
    (void)data;
    (void)src;
    (void)flags;

    return vfs_mount(dest, new_tmpfs());
}

vfs_filesystem_t tmpfs = {
    .name = "tmpfs",
    .mount = tmpfs_mount,
};

vfs_node_t *new_tmpfs(void) {
    return inode_to_node(new_inode(TMPFS_FLAGS_DIR));
}

void init_tmpfs(void) {
    vfs_register_fs(&tmpfs);
    vterm_print("TMPFS...");
    kok();
}

vfs_node_t *tmpfs_lookup(vfs_node_t *node, const char *name) {
    tmpfs_inode_t *inode = (tmpfs_inode_t *)node->private_inode;
    if (!strcmp(name, "."))
        return inode_to_node(inode);
    if (!strcmp(name, "..") && inode->parent)
        return inode_to_node(inode->parent);
    if (!strcmp(name, ".."))
        return inode_to_node(inode);

    foreach (node, inode->entries) {
        tmpfs_dirent_t *entry = node->value;
        if (!strcmp(name, entry->name))
            return inode_to_node(entry->inode);
    }

    return NULL;
}

int64_t tmpfs_read(vfs_node_t *node, void *buffer, uint64_t offset,
                   size_t count) {
    const tmpfs_inode_t *inode = (const tmpfs_inode_t *)node->private_inode;

    if (offset + count > inode->buffer_size) {
        if (offset >= inode->buffer_size)
            return 0;
        count = inode->buffer_size - offset;
    }

    memcpy(buffer, (void *)((uintptr_t)inode->buffer + offset), count);
    return (int64_t)count;
}

int64_t tmpfs_write(vfs_node_t *node, const void *buffer, uint64_t offset,
                    size_t count) {
    tmpfs_inode_t *inode = (tmpfs_inode_t *)node->private_inode;

    if (offset + count > inode->buffer_size)
        tmpfs_truncate(node, offset + count);

    memcpy((void *)((uintptr_t)inode->buffer + offset), buffer, count);

    inode->buffer_size = offset + count;
    node->size = inode->buffer_size;

    return (int64_t)count;
}

int tmpfs_truncate(vfs_node_t *node, size_t size) {
    tmpfs_inode_t *inode = (tmpfs_inode_t *)node->private_inode;
    char *new_buffer = kmalloc(size);

    if (inode->buffer_size > size) {
        memcpy(new_buffer, inode->buffer, size);
    } else {
        memcpy(new_buffer, inode->buffer, inode->buffer_size);
    }

    kfree(inode->buffer);

    inode->buffer_size = size;
    inode->buffer = new_buffer;

    return 0;
}

int tmpfs_unlink(vfs_node_t *node, const char *name) {
    tmpfs_inode_t *inode = (tmpfs_inode_t *)node->private_inode;
    tmpfs_dirent_t *entry = NULL;

    foreach (node, inode->entries) {
        tmpfs_dirent_t *current_entry = node->value;
        if (!strcmp(current_entry->name, name)) {
            entry = current_entry;
            break;
        }
    }

    if (!entry)
        return -1;

    list_remove(inode->entries, entry);
    if ((--entry->inode->link_count) == 0 && entry->inode->open_count == 0)
        free_inode(entry->inode);

    kfree(entry);
    return 0;
}

int tmpfs_link(vfs_node_t *parent_src, const char *src, vfs_node_t *parent_dest,
               const char *dest) {
    tmpfs_inode_t *parent_src_inode =
        (tmpfs_inode_t *)parent_src->private_inode;
    tmpfs_inode_t *parent_dest_inode =
        (tmpfs_inode_t *)parent_dest->private_inode;

    if (tmpfs_exist(parent_dest_inode, dest))
        return -1;

    tmpfs_inode_t *src_inode = NULL;

    foreach (node, parent_src_inode->entries) {
        const tmpfs_dirent_t *entry = node->value;
        if (!strcmp(entry->name, src)) {
            src_inode = entry->inode;
            break;
        }
    }

    if (!src_inode)
        return -1;

    src_inode->link_count++;
    tmpfs_dirent_t *entry = kmalloc(sizeof(tmpfs_dirent_t));
    strcpy(entry->name, dest);
    entry->inode = src_inode;
    list_append(parent_dest_inode->entries, entry);
    return 0;
}

int tmpfs_symlink(vfs_node_t *node, const char *name, const char *target) {
    if (!strcmp(name, ".") || !strcmp(name, ".."))
        return -1;
    tmpfs_inode_t *inode = (tmpfs_inode_t *)node->private_inode;

    if (tmpfs_exist(inode, name))
        return -1;

    tmpfs_inode_t *symlink = new_inode(TMPFS_FLAGS_LINK);
    kfree(symlink->buffer);
    symlink->buffer_size = strlen(target);
    symlink->buffer = strdup(target);

    tmpfs_dirent_t *entry = kmalloc(sizeof(tmpfs_dirent_t));
    strcpy(entry->name, name);
    entry->inode = symlink;
    list_append(inode->entries, entry);

    return 0;
}

ssize_t tmpfs_readlink(vfs_node_t *node, char *buffer, size_t buffer_size) {
    const tmpfs_inode_t *inode = (tmpfs_inode_t *)node->private_inode;

    if (buffer_size > inode->buffer_size)
        buffer_size = inode->buffer_size;

    memcpy(buffer, inode->buffer, buffer_size);
    return buffer_size;
}

struct dirent_t *tmpfs_readdir(vfs_node_t *node, uint64_t index) {
    tmpfs_inode_t *inode = (tmpfs_inode_t *)node->private_inode;

    if (index == 0) {
        struct dirent_t *ret = kmalloc(sizeof(struct dirent_t));
        strcpy(ret->d_name, ".");
        return ret;
    }

    if (index == 1) {
        struct dirent_t *ret = kmalloc(sizeof(struct dirent_t));
        strcpy(ret->d_name, "..");
        return ret;
    }

    index -= 2;

    foreach (node, inode->entries) {
        if (!index) {
            const tmpfs_dirent_t *entry = node->value;
            struct dirent_t *ret = kmalloc(sizeof(struct dirent_t));
            strcpy(ret->d_name, entry->name);
            return ret;
        }

        index--;
    }

    return NULL;
}

void tmpfs_close(vfs_node_t *node) {
    tmpfs_inode_t *inode = (tmpfs_inode_t *)node->private_inode;
    inode->open_count--;
    if (inode->open_count == 0 && inode->link_count == 0)
        free_inode(inode);
}

int tmpfs_create(vfs_node_t *node, const char *name, mode_t perms,
                 uint64_t flags) {
    if (!strcmp(name, ".") || !strcmp(name, ".."))
        return -1;
    tmpfs_inode_t *inode = (tmpfs_inode_t *)node->private_inode;
    if (tmpfs_exist(inode, name))
        return -1;

    uint64_t inode_flag = 0;

    if (flags & VFS_FILE)
        inode_flag |= TMPFS_FLAGS_FILE;
    if (flags & VFS_DIR)
        inode_flag |= TMPFS_FLAGS_DIR;

    tmpfs_inode_t *child_inode = new_inode(inode_flag);
    child_inode->link_count = 1;
    child_inode->parent = inode;
    child_inode->perm = perms;

    tmpfs_dirent_t *entry = kmalloc(sizeof(tmpfs_dirent_t));
    memset(entry, 0, sizeof(tmpfs_dirent_t));
    strcpy(entry->name, name);
    entry->inode = child_inode;
    list_append(inode->entries, entry);

    return 0;
}
