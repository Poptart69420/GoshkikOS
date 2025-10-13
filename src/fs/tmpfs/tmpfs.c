#include <fs/tmpfs/tmpfs.h>

static vfs_t *tmpfs_vfs = NULL;
static tmpfs_node_t *tmpfs_root_node = NULL;

static tmpfs_node_t *tmpfs_alloc_node(const char *name, int type)
{
  tmpfs_node_t *node = kmalloc(sizeof(tmpfs_node_t));
  if (!node)
    return NULL;
  memset(node, 0, sizeof(*node));
  node->name = NULL;
  if (name)
  {
    size_t name_len = strlen(name) + 1;
    node->name = kmalloc(name_len);
    if (!node->name)
    {
      kfree(node);
      return NULL;
    }

    strncpy(node->name, name, name_len);
  }

  node->type = type;
  node->vnode.ops = &tmpfs_vops;
  node->vnode.ref_count = 1;
  node->vnode.flags = 0;
  node->vnode.type = type;
  node->data = NULL;
  node->data_length = 0;

  if (type == V_TYPE_DIR)
  {
    node->attr.type = V_TYPE_DIR;
    node->attr.size = 0;
    node->attr.nlinks = 2;
  }
  else
  {
    node->attr.nlinks = 1;
    node->attr.size = 0;
  }

  return node;
}

static void tmpfs_free_node(tmpfs_node_t *node)
{
  if (!node)
    return;
  if (node->name)
    kfree(node->name);
  if (node->data)
    kfree(node->data);
  kfree(node);
}

static tmpfs_node_t *tmpfs_find_child(tmpfs_node_t *dir, const char *name)
{
  if (!dir || dir->type != V_TYPE_DIR)
    return NULL;
  tmpfs_node_t *child = dir->children;
  while (child)
  {
    if (child->name && strcmp(child->name, name) == 0)
      return child;
    child = child->next;
  }

  return NULL;
}

static int tmpfs_attach_child(tmpfs_node_t *parent, tmpfs_node_t *child)
{
  if (!parent || parent->type != V_TYPE_DIR)
    return -EINVAL;
  child->parent = parent;
  child->next = parent->children;
  parent->children = child;
  parent->attr.nlinks++;
  return 0;
}

static tmpfs_node_t *tmpfs_detach_child(tmpfs_node_t *parent, const char *name)
{
  if (!parent || parent->type != V_TYPE_DIR)
    return NULL;
  tmpfs_node_t *prev = NULL;
  tmpfs_node_t *current = parent->children;
  while (current)
  {
    if (current->name && strcmp(current->name, name) == 0)
    {
      if (prev)
        prev->next = current->next;
      else
        parent->children = current->next;
      parent->attr.nlinks--;
      current->next = NULL;
      return current;
    }
    prev = current;
    current = current->next;
  }
  return NULL;
}

static tmpfs_node_t *to_tmpfs(vnode_t *vnode)
{
  return vnode ? (tmpfs_node_t *)vnode : NULL;
}

// vops

static int tmpfs_mount(vfs_t **vfs_out, vnode_t *mount_point, vnode_t *backing, void *data)
{
  (void)backing;
  (void)data;

  *vfs_out = kmalloc(sizeof(vfs_t));
  if (!*vfs_out)
    return -ENOMEM;

  memset(*vfs_out, 0, sizeof(vfs_t));
  (*vfs_out)->ops = &tmpfs_vfs_ops;
  (*vfs_out)->node_covered = mount_point;
  (*vfs_out)->flags = V_FLAGS_ROOT;

  tmpfs_root_node = tmpfs_alloc_node("/", V_TYPE_DIR);
  if (!tmpfs_root_node)
    return -ENOMEM;

  (*vfs_out)->root = &tmpfs_root_node->vnode;
  tmpfs_vfs = *vfs_out;
  return 0;
}

static int tmpfs_unmount(vfs_t *vfs)
{
  (void)vfs;
  return 0;
}

static int tmpfs_root(vfs_t *vfs, vnode_t **root)
{
  (void)vfs;
  *root = &tmpfs_root_node->vnode;
  return 0;
}

static int tmpfs_open(vnode_t **node, int flags, cred_t *cred)
{
  (void)flags;
  (void)cred;
  if (!node || !*node)
    return -EINVAL;
  (*node)->ref_count++;
  return 0;
}

static int tmpfs_close(vnode_t *node, int flags, cred_t *cred)
{
  (void)flags;
  (void)cred;
  if (node && node->ref_count > 0)
    node->ref_count--;
  return 0;
}

static int tmpfs_lookup(vnode_t *dir_vnode, char *name, vnode_t **result, cred_t *cred)
{
  (void)cred;
  tmpfs_node_t *dir = to_tmpfs(dir_vnode);
  if (!dir || dir->type != V_TYPE_DIR)
    return -ENOTDIR;
  if (!name || !*name)
    return -EINVAL;

  tmpfs_node_t *child = tmpfs_find_child(dir, name);
  if (!child)
    return -ENOENT;

  *result = &child->vnode;
  return 0;
}

static int tmpfs_create(vnode_t *parent_vnode, char *name, vattr_t *attr, int type, vnode_t **result, cred_t *cred)
{
  (void)cred;
  tmpfs_node_t *parent = to_tmpfs(parent_vnode);
  if (!parent || parent->type != V_TYPE_DIR)
    return -ENOTDIR;
  if (tmpfs_find_child(parent, name))
    return -EEXIST;

  tmpfs_node_t *node = tmpfs_alloc_node(name, type);
  if (!node)
    return -ENOMEM;

  tmpfs_attach_child(parent, node);
  if (attr)
    memcpy(&node->attr, attr, sizeof(vattr_t));
  *result = &node->vnode;
  return 0;
}

static int tmpfs_read(vnode_t *node, size_t size, uintmax_t offset, int flags,
                      size_t *readc, cred_t *cred, void *buffer)
{
  (void)flags;
  (void)cred;
  tmpfs_node_t *tmpfs_node = to_tmpfs(node);
  if (!tmpfs_node || tmpfs_node->type != V_TYPE_REGULAR)
    return -EINVAL;
  if (offset >= tmpfs_node->data_length)
  {
    *readc = 0;
    return 0;
  }
  size_t remain = tmpfs_node->data_length - offset;
  size_t nread = (size < remain) ? size : remain;
  memcpy(buffer, (char *)tmpfs_node->data + offset, nread);
  *readc = nread;
  return 0;
}

static int tmpfs_write(vnode_t *node, size_t size, uintmax_t offset, int flags,
                       size_t *writec, cred_t *cred, void *buffer)
{
  (void)flags;
  (void)cred;
  tmpfs_node_t *tmpfs_node = to_tmpfs(node);
  if (!tmpfs_node || tmpfs_node->type != V_TYPE_REGULAR)
    return -EINVAL;
  if (offset + size > tmpfs_node->data_length)
  {
    void *newbuf = kmalloc(offset + size);
    if (!newbuf)
      return -ENOMEM;
    if (tmpfs_node->data)
    {
      memcpy(newbuf, tmpfs_node->data, tmpfs_node->data_length);
      kfree(tmpfs_node->data);
    }
    tmpfs_node->data = newbuf;
    tmpfs_node->data_length = offset + size;
  }
  memcpy((char *)tmpfs_node->data + offset, buffer, size);
  *writec = size;
  return 0;
}

// op tables

vfs_ops_t tmpfs_vfs_ops = {
    .mount = tmpfs_mount,
    .unmount = tmpfs_unmount,
    .sync = NULL,
    .root = tmpfs_root,
};

vops_t tmpfs_vops = {
    .open = tmpfs_open,
    .close = tmpfs_close,
    .read = tmpfs_read,
    .write = tmpfs_write,
    .lookup = NULL,
    .create = tmpfs_create,
    .getattr = NULL,
    .setattr = NULL,
    .access = NULL,
    .unlink = NULL,
    .link = NULL,
    .symlink = NULL,
    .readlink = NULL,
    .inactive = NULL,
    .mmap = NULL,
    .munmap = NULL,
    .isatty = NULL,
    .maxseek = NULL,
    .resize = NULL,
    .rename = NULL,
    .sync = NULL,
};
