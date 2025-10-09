#include <fs/vfs/vfs.h>

static vfs_t *vfs_list = NULL;
vnode_t *vfs_root = NULL;

vnode_t *vnode_alloc(vops_t *ops, int type, vfs_t *vfs)
{
  vnode_t *vnode = (vnode_t *)kmalloc(sizeof(vnode_t));
  if (!vnode)
    return NULL;
  vnode->ops = ops;
  vnode->ref_count = 1;
  vnode->flags = 0;
  vnode->type = type;
  vnode->vfs = vfs;
  vnode->vfs_mounted = NULL;
  return vnode;
}

void vnode_hold(vnode_t *vnode)
{
  if (!vnode)
    return;
  vnode->ref_count++;
}

void vnode_put(vnode_t *vnode)
{
  if (!vnode)
    return;
  vnode->ref_count--;
  if (vnode->ref_count <= 0)
  {
    if (vnode->ops && vnode->ops->inactive)
      vnode->ops->inactive(vnode);
    kfree(vnode);
  }
}

static char *next_component(char **path_p)
{
  char *p = *path_p;
  if (!p)
    return NULL;

  while (*p == '/')
    p++;

  if (!*p)
    return NULL;

  char *start = p;
  while (*p && *p != '/')
    p++;

  if (*p)
  {
    *p = '\0';
    p++;
  }

  *path_p = p;
  return start;
}

int vfs_register(vfs_t **out, vfs_ops_t *ops)
{
  if (!ops || !out)
    return -EINVAL;

  vfs_t *vfs = (vfs_t *)kmalloc(sizeof(vfs_t));
  if (!vfs)
    return -ENOMEM;
  vfs->next = vfs_list;
  vfs->ops = ops;
  vfs->node_covered = NULL;
  vfs->root = NULL;
  vfs->flags = 0;
  vfs_list = vfs;
  *out = vfs;
  return 0;
}

static vfs_t *vfs_find_by_ops(vfs_ops_t *ops)
{
  vfs_t *vfs = vfs_list;
  while (vfs)
  {
    if (vfs->ops == ops)
      return vfs;
    vfs = vfs->next;
  }

  return NULL;
}

int vfs_mount(vfs_t **out, vfs_ops_t *ops, vnode_t *mount_point, vnode_t *backing, void *data)
{
  if (!ops || !out || !mount_point)
    return -EINVAL;

  int rc = 0;

  vfs_t *vfs = vfs_find_by_ops(ops);
  if (!vfs)
  {
    rc = vfs_register(&vfs, ops);
    if (rc)
      return rc;
  }

  if (!vfs->ops->mount)
    return -ENOSYS;

  rc = vfs->ops->mount(&vfs, mount_point, backing, data);
  if (rc)
    return rc;
  vfs->node_covered = mount_point;
  mount_point->vfs_mounted = vfs;
  *out = vfs;

  return rc;
}

int vfs_unmount(vfs_t *vfs)
{
  if (!vfs)
    return -EINVAL;
  if (!vfs->ops || !vfs->ops->unmount)
    return -ENOSYS;
  int rc = vfs->ops->unmount(vfs);
  if (rc)
    return rc;
  if (vfs->node_covered)
  {
    vfs->node_covered->vfs_mounted = NULL;
    vfs->node_covered = NULL;
  }

  if (vfs_list == vfs)
  {
    vfs_list = vfs->next;
  }
  else
  {
    vfs_t *p = vfs_list;
    while (p && p->next != vfs)
      p = p->next;
    if (p)
      p->next = vfs->next;
  }

  kfree(vfs);
  return rc;
}

int vfs_get_root(vfs_t *vfs, vnode_t **root_out)
{
  if (!vfs || !root_out)
    return -EINVAL;
  if (vfs->ops && vfs->ops->root)
    return vfs->ops->root(vfs, root_out);
  if (vfs->root)
  {
    vnode_hold(vfs->root);
    *root_out = vfs->root;
    return 0;
  }

  return -ENOENT;
}

int vfs_resolve_absolute(const char *path, vnode_t **res, cred_t *cred)
{
  if (!path || !res)
    return -EINVAL;
  if (!vfs_root)
    return -ENOENT;

  char buffer[PATHNAME_MAX];
  size_t len = strnlen(path, PATHNAME_MAX);
  if (len >= PATHNAME_MAX)
    return -ENAMETOOLONG;
  strncpy(buffer, path, PATHNAME_MAX);

  vnode_t *current = vfs_root;
  vnode_hold(current);

  char *p = buffer;
  char *comp;
  int symlink_depth = 0;

  while ((comp = next_component(&p)) != NULL)
  {
    if (strcmp(comp, ".") == 0)
      continue;
    if (strcmp(comp, "..") == 0)
    {
      vnode_t *parent = current->vfs_mounted ? current->vfs_mounted->node_covered : NULL;
      if (parent)
      {
        vnode_hold(parent);
        vnode_put(current);
        current = parent;
      }
      continue;
    }

    if (!current->ops || !current->ops->lookup)
    {
      vnode_put(current);
      return -ENOTDIR;
    }

    vnode_t *next = NULL;
    int rc = current->ops->lookup(current, comp, &next, cred);
    if (rc)
    {
      vnode_put(current);
      return rc;
    }
    vnode_put(current);
    if (!next)
      return -ENOENT;

    while (next->type == V_TYPE_LINK)
    {
      if (symlink_depth++ >= MAXLINKDEPTH)
      {
        vnode_put(next);
        return -ELOOP;
      }

      char *target = NULL;
      if (!next->ops || !next->ops->readlink)
      {
        vnode_put(next);
        return -EINVAL;
      }

      rc = next->ops->readlink(next, &target, cred);
      vnode_put(next);
      if (rc)
        return rc;

      rc = vfs_resolve_absolute(target, &next, cred);
      kfree(target);
      if (rc)
        return rc;
    }

    current = next;
  }

  *res = current;
  return 0;
}

int vnode_lookup(vnode_t *dir, const char *name, vnode_t **result, cred_t *cred)
{
  if (!dir || !name || !result)
    return -EINVAL;
  if (!dir->ops->lookup)
    return -ENOSYS;
  return dir->ops->lookup(dir, (char *)name, result, cred);
}

int vfs_open(vnode_t **node, int flags, cred_t *cred)
{
  if (!node || !*node)
    return -EINVAL;

  vnode_t *vnode = *node;
  if (!vnode->ops || !vnode->ops->open)
    return -ENOSYS;
  return vnode->ops->open(node, flags, cred);
}

int vfs_close(vnode_t *node, int flags, cred_t *cred)
{
  if (!node)
    return -EINVAL;
  if (!node->ops || !node->ops->close)
    return -ENOSYS;
  return node->ops->close(node, flags, cred);
}

int vfs_read(vnode_t *node, size_t size, uintmax_t offset, int flags, size_t *readc, cred_t *cred)
{
  if (!node)
    return -EINVAL;
  if (!node->ops || !node->ops->read)
    return -ENOSYS;
  return node->ops->read(node, size, offset, flags, readc, cred);
}

int vfs_write(vnode_t *node, size_t size, uintmax_t offset, int flags, size_t *writec, cred_t *cred)
{
  if (!node)
    return -EINVAL;
  if (!node->ops || !node->ops->write)
    return -ENOSYS;
  return node->ops->write(node, size, offset, flags, writec, cred);
}

int vfs_create(vnode_t *parent, char *name, vattr_t *attr, int type, vnode_t **result, cred_t *cred)
{
  if (!parent || !name)
    return -EINVAL;
  if (!parent->ops || !parent->ops->create)
    return -ENOSYS;
  return parent->ops->create(parent, name, attr, type, result, cred);
}

int vfs_getattr(vnode_t *node, vattr_t *attr, cred_t *cred)
{
  if (!node || !attr)
    return -EINVAL;
  if (!node->ops || !node->ops->getattr)
    return -ENOSYS;
  return node->ops->getattr(node, attr, cred);
}

int vfs_setattr(vnode_t *node, vattr_t *attr, int attrs, cred_t *cred)
{
  if (!node || !attr)
    return -EINVAL;
  if (!node->ops || !node->ops->setattr)
    return -ENOSYS;
  return node->ops->setattr(node, attr, attrs, cred);
}

int vfs_set_root(vnode_t *root)
{
  if (!root)
    return -EINVAL;
  if (vfs_root)
    vnode_put(vfs_root);
  vnode_hold(root);
  vfs_root = root;
  return 0;
}

int init_vfs(void)
{
  vterm_print("VFS...");
  vfs_list = NULL;
  vfs_root = NULL;
  kok();
  return 0;
}
