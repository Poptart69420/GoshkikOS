#include "tmpfs.h"

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
    node->flags - 0;

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

    node->close = tmpfs_close;
    inode->open_count++;
    return node;
}
