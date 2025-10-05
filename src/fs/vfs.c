#include <fs/vfs/vfs.h>

static hashtable_t fs_table;
vfs_dirent_t *vfs_root;

static vfs_dirent_t *vfs_resolve_start(vfs_dirent_t *at, const char *path) {
    if (!path || !*path)
        return NULL;
    if (path[0] == '/')
        return vfs_root;
    return at ? at : vfs_root;
}

void init_vfs(void) {
    vterm_print("VFS...");
    hashtable_init(&fs_table, 20);
    vfs_root = kmalloc(sizeof(vfs_dirent_t));

    if (!vfs_root) {
        kerror("Failed to allocate vfs root");
        hcf();
    }

    memset(vfs_root, 0, sizeof(vfs_dirent_t));
    vfs_root->name = strdup("/");
    vfs_root->inode = kmalloc(sizeof(vfs_inode_t));

    if (!vfs_root->inode) {
        kerror("Failed to allocate inode for vfs root");
        hcf();
    }

    memset(vfs_root->inode, 0, sizeof(vfs_inode_t));
    vfs_root->inode->type = VFS_DIR;
    vfs_root->ref_count = 1;
    vfs_root->parent = vfs_root;
    vfs_root->inode->ref_count = 1;
    vfs_root->inode->flags = VFS_FLAG_READ | VFS_FLAG_write;
    vfs_root->attributes.type = VFS_DIR;
    vfs_root->attributes.mode = 0755;

    kok();
}

int vfs_register(const char *name, vfs_dirent_t *root) {
    if (!name || !root)
        return -1;

    void *existing = NULL;
    if (hashtable_get(&fs_table, &existing, (void *)name, strlen(name) + 1) ==
            0 &&
        existing)
        return -1;

    return hashtable_set(&fs_table, (void *)root, (void *)name,
                         strlen(name) + 1);
}

int vfs_create(vfs_dirent_t *parent, int type, const char *path) {
    if (!path || !*path)
        return -1;
    if (!vfs_root)
        return -1;
    if (!parent)
        parent = vfs_root;

    char *buffer = strdup(path);
    if (!buffer)
        return -1;

    char *token = strtok(buffer, "/");
    vfs_dirent_t *current = parent;

    while (token) {
        char *next = strtok(NULL, "/");

        vfs_dirent_t *child = current->child;
        while (child) {
            if (strcmp(child->name, token) == 0)
                break;
            child = child->next_sibling;
        }

        if (!child) {
            int new_type = next ? VFS_DIR : type;

            vfs_dirent_t new_entry;
            memset(&new_entry, 0, sizeof(vfs_dirent_t));
            new_entry.name = token;
            new_entry.attributes.type = new_type;

            if (!current->inode || !current->inode->ops) {
                kfree(buffer);
                return -1;
            }

            vfs_inode_t *new_inode = NULL;
            int ret;

            if (new_type == VFS_DIR) {
                if (!current->inode->ops->mkdir) {
                    kfree(buffer);
                    return -1;
                }
                ret = current->inode->ops->mkdir(current->inode, &new_entry);
                new_inode = new_entry.inode;
            } else {
                if (!current->inode->ops->create) {
                    kfree(buffer);
                    return -1;
                }
                ret = current->inode->ops->create(current->inode, &new_entry,
                                                  &new_inode);
            }

            if (ret != 0 || !new_inode) {
                kfree(buffer);
                return -1;
            }

            vfs_dirent_t *entry_copy = kmalloc(sizeof(vfs_dirent_t));
            if (!entry_copy) {
                kfree(buffer);
                return -1;
            }

            memcpy(entry_copy, &new_entry, sizeof(vfs_dirent_t));
            entry_copy->inode = new_inode;
            entry_copy->parent = current;
            entry_copy->next_sibling = current->child;
            current->child = entry_copy;

            child = entry_copy;
        }

        current = child;
        token = next;
    }

    kfree(buffer);
    return 0;
}

int vfs_unregister(const char *name) {
    if (!name)
        return -1;
    void *value = NULL;
    if (hashtable_get(&fs_table, &value, (void *)name, strlen(name) + 1) != 0 ||
        !value)
        return -1;
    return hashtable_remove(&fs_table, (void *)name, strlen(name) + 1);
}

int vfs_lookup(vfs_dirent_t **out, vfs_dirent_t *at, const char *path,
               vfs_flags_t flags) {
    if (!path || !*path)
        return -1;

    vfs_dirent_t *current = vfs_resolve_start(at, path);
    if (!current)
        return -1;

    char *buffer = strdup(path);
    if (!buffer)
        return -1;

    char *saveptr;
    char *token = strtok_r(buffer, "/", &saveptr);

    while (token) {
        if (strcmp(token, ".") == 0) {
            token = strtok_r(NULL, "/", &saveptr);
            continue;
        }

        if (strcmp(token, "..") == 0) {
            current = current->parent ? current->parent : current;
            token = strtok_r(NULL, "/", &saveptr);
            continue;
        }

        vfs_dirent_t *child = current->child;
        while (child && strcmp(child->name, token) != 0)
            child = child->next_sibling;

        if (!child) {
            if (!current->inode || !current->inode->ops ||
                !current->inode->ops->lookup) {
                kfree(buffer);
                return -1;
            }

            vfs_dirent_t tmp;
            memset(&tmp, 0, sizeof(tmp));
            tmp.name = token;

            if (current->inode->ops->lookup(current->inode, &tmp) != 0) {
                if (flags & VFS_FLAG_LOOKUP_CREATE_PARENTS) {
                    if (vfs_create(current, VFS_DIR, token) != 0) {
                        kfree(buffer);
                        return -1;
                    }
                    child = current->child;
                    while (child && strcmp(child->name, token) != 0)
                        child = child->next_sibling;
                    if (!child) {
                        kfree(buffer);
                        return -1;
                    }
                } else {
                    kfree(buffer);
                    return -1;
                }
            } else {
                vfs_dirent_t *new_entry = kmalloc(sizeof(vfs_dirent_t));
                if (!new_entry) {
                    kfree(buffer);
                    return -1;
                }
                memcpy(new_entry, &tmp, sizeof(vfs_dirent_t));
                new_entry->parent = current;
                new_entry->next_sibling = current->child;
                current->child = new_entry;
                child = new_entry;
            }
        }

        current = child;
        token = strtok_r(NULL, "/", &saveptr);
    }

    kfree(buffer);
    ++current->ref_count;
    *out = current;
    return 0;
}

int vfs_open(vfs_file_t **out, vfs_dirent_t *at, const char *path,
             vfs_flags_t flags) {
    if (!out || !path || !*path)
        return -1;

    vfs_dirent_t *target = NULL;
    if (vfs_lookup(&target, at, path, flags) != 0)
        return -1;

    if (!target->inode) {
        --target->ref_count;
        return -1;
    }

    if (target->inode->type != VFS_REG) {
        --target->ref_count;
        return -1;
    }

    vfs_file_t *file = kmalloc(sizeof(vfs_file_t));
    if (!file) {
        --target->ref_count;
        return -1;
    }

    memset(file, 0, sizeof(vfs_file_t));
    file->inode = target->inode;
    file->attributes = target->attributes;
    file->offset = 0;
    file->ref_count = 1;
    file->private = target->private;

    if (target->inode->private)
        file->ops = (const vfs_file_ops_t *)target->inode->private;
    else if (target->private)
        file->ops = (const vfs_file_ops_t *)target->private;
    else
        file->ops = NULL;

    if (file->ops && file->ops->open) {
        int r = file->ops->open(file);
        if (r != 0) {
            kfree(file);
            --target->ref_count;
            return r;
        }
    }

    *out = file;
    return 0;
}

int vfs_close(vfs_file_t *file) {
    if (!file)
        return -1;
    if (file->ops && file->ops->close)
        file->ops->close(file);
    --file->inode->ref_count;
    kfree(file);
    return 0;
}

int vfs_unlink(vfs_dirent_t *parent, const char *path) {
    if (!parent || !path || !*path)
        return -1;

    vfs_dirent_t *target = NULL;
    if (vfs_lookup(&target, parent, path, 0) != 0)
        return -1;

    if (!target->inode || target->attributes.type != VFS_REG) {
        --target->ref_count;
        return -1;
    }

    if (!parent->inode || !parent->inode->ops || !parent->inode->ops->unlink) {
        --target->ref_count;
        return -1;
    }

    int ret = parent->inode->ops->unlink(parent->inode, target);
    if (ret != 0) {
        --target->ref_count;
        return ret;
    }

    vfs_dirent_t **link = &parent->child;
    while (*link) {
        if (*link == target) {
            *link = target->next_sibling;
            break;
        }
        link = &(*link)->next_sibling;
    }

    kfree(target->name);
    if (target->inode)
        kfree(target->inode);
    kfree(target);

    return 0;
}

int vfs_rmdir(vfs_dirent_t *parent, const char *path) {
    if (!parent || !path || !*path)
        return -1;

    vfs_dirent_t *target = NULL;
    if (vfs_lookup(&target, parent, path, 0))
        return -1;

    if (!target->inode || target->attributes.type != VFS_DIR) {
        --target->ref_count;
        return -1;
    }

    if (target->child) {
        --target->ref_count;
        return -1;
    }

    if (!parent->inode || !parent->inode->ops || !parent->inode->ops->rmdir) {
        --target->ref_count;
        return -1;
    }

    int ret = parent->inode->ops->rmdir(parent->inode, target);
    if (ret) {
        --target->ref_count;
        return ret;
    }

    vfs_dirent_t **link = &parent->child;
    while (*link) {
        if (*link == target) {
            *link = target->next_sibling;
            break;
        }

        link = &(*link)->next_sibling;
    }

    kfree(target->name);
    kfree(target->inode);
    kfree(target);
    return 0;
}

ssize_t vfs_read(vfs_file_t *file, void *buf, size_t len, vfs_off_t off) {
    if (!file || !file->inode || !buf || len == 0)
        return -1;

    if (file->inode->type != VFS_REG)
        return -1;

    if (!file->ops || !file->ops->read)
        return -1;

    ssize_t ret = file->ops->read(file, buf, len, off);
    if (ret < 0)
        return ret;

    file->offset += ret;
    return ret;
}
ssize_t vfs_write(vfs_file_t *file, const void *buf, size_t len,
                  vfs_off_t off) {
    if (!file || !file->inode || !buf || len == 0)
        return -1;

    if (file->inode->type != VFS_REG)
        return -1;

    if (!file->ops || !file->ops->write)
        return -1;

    ssize_t ret = file->ops->write(file, buf, len, off);
    if (ret < 0)
        return ret;

    file->offset += ret;
    return ret;
}

ssize_t vfs_read_offset(vfs_file_t *file, void *buf, size_t len) {
    return vfs_read(file, buf, len, file->offset);
}

ssize_t vfs_write_offset(vfs_file_t *file, const void *buf, size_t len) {
    return vfs_write(file, buf, len, file->offset);
}
