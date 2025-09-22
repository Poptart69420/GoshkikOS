#include "vfs.h"

static filesystem_t *registered_filesystems[MAX_FILESYSTEMS];
static mount_t mounts[MAX_MOUNTS];

void init_vfs(void)
{
  for (int i = 0; i < MAX_FILESYSTEMS; ++i) {
    registered_filesystems[i] = NULL;
  }

  for (int i = 0; i < MAX_MOUNTS; ++i) {
    mounts[i].root_node = NULL;
    mounts[i].path[0] = '\0';
  }

  vterm_print("VFS:   Initialized\n");
}

int vfs_register_filesystem(filesystem_t *fs)
{
  vterm_print("VFS:   Loading ");
  vterm_print(fs->name);
  vterm_print("\n");

  for (int i = 0; i < MAX_FILESYSTEMS; ++i) {
    if (!registered_filesystems[i]) {
      registered_filesystems[i] = fs;
      return 0;
    }
  }

  return -1;
}

int vfs_mount(const char *fs_name, void *mount_data, const char *mount_path)
{
  char tmp[256];
  strncpy(tmp, mount_path, sizeof(tmp));
  tmp[sizeof(tmp) - 1] = '\0';

  size_t tlen = strlen(tmp);
  if (tlen > 1 && tmp[tlen - 1] == '/')
    tmp[tlen - 1] = '\0';

  vterm_print("VFS:   Mount ");
  vterm_print(fs_name);
  vterm_print(" at ");
  vterm_print(tmp);
  vterm_print("\n");

  for (int i = 0; i < MAX_FILESYSTEMS; ++i) {
    filesystem_t *fs = registered_filesystems[i];
    if (fs && strcmp(fs->name, fs_name) == 0) {
      if (fs->init) fs->init();
      vfs_node_t *root = fs->mount(mount_data);
      if (!root) return -1;

      for (int j = 0; j < MAX_MOUNTS; ++j) {
        if (!mounts[j].root_node) {
          strncpy(mounts[j].path, tmp, sizeof(mounts[j].path));
          mounts[j].path[sizeof(mounts[j].path) - 1] = '\0';
          mounts[j].root_node = root;
          return 0;
        }
      }
    }
  }

  return -1;
}

static vfs_node_t *vfs_resolve(const char *path)
{
  char tmp[256];
  strncpy(tmp, path, sizeof(tmp));
  tmp[sizeof(tmp) - 1] = '\0';

  const mount_t *best = NULL;
  size_t best_length = 0;
  size_t path_length = strlen(tmp);

  for (int i = 0; i < MAX_MOUNTS; ++i) {
    if (!mounts[i].root_node) continue;
    size_t length = strlen(mounts[i].path);
    if (length > path_length) continue;
    if (strncmp(tmp, mounts[i].path, length) == 0) {
      if (tmp[length] == '\0' || tmp[length] == '/' ||
          (length == 1 && mounts[i].path[0] == '/')) {
        if (length > best_length) {
          best = &mounts[i];
          best_length = length;
        }
      }
    }
  }

  if (!best) return NULL;

  path_t parsed;
  const char *sub = tmp + best_length;
  if (*sub == '/') ++sub;
  path_parse(sub, &parsed);

  vfs_node_t *node = best->root_node;

  for (size_t i = 0; i < parsed.count; ++i) {
    if (!node->ops || !node->ops->finddir) return NULL;
    node = node->ops->finddir(node, parsed.parts[i]);
    if (!node) return NULL;
  }

  return node;
}

vfs_node_t *vfs_root(void)
{
  return vfs_resolve("/");
}

vfs_node_t *vfs_lookup(const char *path)
{
  return vfs_resolve(path);
}

size_t vfs_read(vfs_node_t *node, size_t offset, size_t size, void *buffer)
{
  if (!node || !node->ops || !node->ops->read) return -1;

  return node->ops->read(node, offset, size, buffer);
}

size_t vfs_write(vfs_node_t *node, size_t offset, size_t size, const void *buffer)
{
  if (!node || !node->ops || !node->ops->write) return -1;

  return node->ops->write(node, offset, size, buffer);

}

int vfs_open(vfs_node_t *node)
{
  if (!node || !node->ops || !node->ops->open) return -1;

  return node->ops->open(node);
}

int vfs_close(vfs_node_t *node)
{
  if (!node || !node->ops || !node->ops->close) return -1;

  return node->ops->close(node);
}

int vfs_readdir(vfs_node_t *node, size_t index, vfs_dirent_t *dirent)
{
  if (!node || !node->ops || !node->ops->readdir) return -1;

  return node->ops->readdir(node, index, dirent);
}

vfs_node_t *vfs_finddir(vfs_node_t *node, const char *name)
{
  if (!node || !node->ops || !node->ops->finddir) return NULL;

  return node->ops->finddir(node, name);
}

vfs_node_t *vfs_create_file(const char *path, const void *content, size_t size)
{
  char tmp[256];
  strncpy(tmp, path, sizeof(tmp));
  tmp[sizeof(tmp) - 1] = '\0';

  char *last = strrchr(tmp, '/');
  if (!last) return NULL;

  *last = '\0';
  const char *name = last + 1;

  vfs_node_t *parent = vfs_lookup(tmp[0] ? tmp : "/");
  if (!parent || !parent->ops || !parent->ops->create) return NULL;

  return parent->ops->create(parent, name, false, content, size);
}

vfs_node_t *vfs_create_dir(const char *path)
{
  if (!path || !*path) return NULL;

  char tmp[256];
  strncpy(tmp, path, sizeof(tmp));
  tmp[sizeof(tmp) - 1] = '\0';

  if (strcmp(tmp, "/") == 0) return NULL;

  char *last = strrchr(tmp, '/');
  if (!last) return NULL;

  const char *name = last + 1;
  *last = '\0';

  vfs_node_t *parent = vfs_lookup(tmp[0] ? tmp : "/");
  if (!parent || !parent->ops || !parent->ops->create) return NULL;

  return parent->ops->create(parent, name, true, NULL, 0);
}

int vfs_rmf(const char *path)
{
  char tmp[256];
  strncpy(tmp, path, sizeof(tmp));
  tmp[sizeof(tmp) - 1] = '\0';

  char *last = strrchr(tmp, '/');
  if (!last) return -1;

  const char *name = last + 1;
  *last = '\0';

  vfs_node_t *parent = vfs_lookup(tmp[0] ? tmp : "/");
  if (!parent || !parent->ops || !parent->ops->rmf) return -1;

  return parent->ops->rmf(parent, name);
}

int vfs_rmdir(const char *path)
{
  char tmp[256];
  strncpy(tmp, path, sizeof(tmp));
  tmp[sizeof(tmp) - 1] = '\0';

  char *last = strrchr(tmp, '/');
  if (!last) return -1;

  const char *name = last + 1;
  *last = '\0';

  vfs_node_t *parent = vfs_lookup(tmp[0] ? tmp : "/");
  if (!parent || !parent->ops || !parent->ops->rmdir) return -1;

  return parent->ops->rmdir(parent, name);
}

int vfs_remove(const char *path)
{
  if (!path || !*path) return -1;

  char tmp[256];
  strncpy(tmp, path, sizeof(tmp));
  tmp[sizeof(tmp) - 1] = '\0';

  const vfs_node_t *node = vfs_lookup(tmp);
  if (!node) return -1;

  char *last = strrchr(tmp, '/');
  if (!last) return -1;

  const char *name = last + 1;
  *last = '\0';

  vfs_node_t *parent = vfs_lookup(tmp[0] ? tmp : "/");
  if (!parent || !parent->ops) return -1;

  switch (node->type) {
    case VFS_NODE_DIR:
      if (parent->ops->rmdir) return parent->ops->rmdir(parent, name);
      break;
    case VFS_NODE_FILE:
      if (parent->ops->rmf) return parent->ops->rmf(parent, name);
      break;
    default:
      return -1;
      break;
  }

  return -1;
}

void vfs_ls(const char *path)
{
  vfs_node_t *dir = vfs_lookup(path);

  if (!dir || dir->type != VFS_NODE_DIR) {
    vterm_print("Not a directory\n");
    return;
  }

  vfs_dirent_t entry;
  size_t index = 0;

  while (vfs_readdir(dir, index, &entry) == 0) {
    vterm_print(entry.name);
    vterm_print(" ");
    index++;
  }
}
