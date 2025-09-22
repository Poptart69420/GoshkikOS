#include "ramfs.h"

// Forward declaration

static vfs_node_t *ramfs_mount(void *data);
static vfs_node_t *ramfs_finddir(vfs_node_t *node, const char *name);
static int        ramfs_readdir(vfs_node_t *node, size_t index, vfs_dirent_t *dirent);
static size_t     ramfs_read(vfs_node_t *node, size_t offset, size_t size, void *buffer);
static size_t     ramfs_write(vfs_node_t *node, size_t offset, size_t size, const void *buffer);
static int        ramfs_open(vfs_node_t *node);
static int        ramfs_close(vfs_node_t *node);
static vfs_node_t *ramfs_create_node(vfs_node_t *parent_node, const char *name, bool is_dir, const void *content, size_t size);
static int        ramfs_rmf(vfs_node_t *parent_node, const char *name);
static int        ramfs_rmdir(vfs_node_t *parent_node, const char *name);
static vfs_ops_t  ramfs_ops;

// Structs

filesystem_t fs_ramfs =
{
  .name  = "ramfs",
  .init  = NULL,
  .mount = ramfs_mount
};

static vfs_ops_t ramfs_ops =
{
  .read = ramfs_read,
  .write   = ramfs_write,
  .open    = ramfs_open,
  .close   = ramfs_close,
  .readdir = ramfs_readdir,
  .finddir = ramfs_finddir,
  .create  = ramfs_create_node,
  .rmf     = ramfs_rmf,
  .rmdir   = ramfs_rmdir
};

// Functions

static void ramfs_init_node_file(ramfs_file_t *file)
{
  if (!file) return;

  memset(&file->node, 0, sizeof(vfs_node_t));
  strncpy(file->node.name, file->name[0] ? file->name : "/", sizeof(file->node.name));
  file->node.name[sizeof(file->node.name) - 1] = '\0';

  file->node.type = file->is_dir ? VFS_NODE_DIR : VFS_NODE_FILE;
  file->node.permissions = (file->is_dir ? (VFS_READ | VFS_EXEC) : (VFS_READ | VFS_WRITE));
  file->node.size = file->size;
  file->node.private_data = file;
  file->node.ops = &ramfs_ops;
  file->node.parent = NULL;
}

static vfs_node_t *ramfs_mount(void *data)
{
  (void) data;
  ramfs_file_t *root_file = kmalloc(sizeof(ramfs_file_t));
  if (!root_file) return NULL;
  memset(root_file, 0, sizeof(ramfs_file_t));
  root_file->name[0] = '\0';
  root_file->is_dir = true;
  root_file->parent = NULL;
  root_file->children = NULL;
  root_file->next = NULL;
  ramfs_init_node_file(root_file);
  root_file->node.parent = NULL;
  return &root_file->node;
}

// Ops functions

static size_t ramfs_read(vfs_node_t *node, size_t offset, size_t size, void *buffer)
{
  if (!node || !buffer) return -1;
  const ramfs_file_t *file = (ramfs_file_t *)node->private_data;

  if(!file) return -1;
  if (file->is_dir) return -2;
  if (!file->data || offset >=file->size) return -1;

  size_t avail = file->size - offset;
  size_t to_read = (size < avail) ? size : avail;

  memcpy(buffer, file->data + offset, to_read);
  return to_read;
}

static size_t ramfs_write(vfs_node_t *node, size_t offset, size_t size, const void *buffer)
{
  if (!node || !buffer) return -1;

  ramfs_file_t *file = (ramfs_file_t *)node->private_data;

  if (!file) return -1;
  if (file->is_dir) return -1;

  size_t end = offset + size;

  if (end > file->size) {
    uint8_t *new_data = kmalloc(end);
    if (!new_data) return -1;

    if (file->data) {
      memcpy(new_data, file->data, file->size);
      kfree(file->data);
    } else {
      memset(new_data, 0, end);
    }

    if (offset > file->size) {
      memset(new_data + file->size, 0, offset - file->size);
    }

    file->data = new_data;
    file->size = end;
    file->node.size = file->size;
  }

  memcpy(file->data + offset, buffer, size);
  return size;
}

static int ramfs_open(vfs_node_t *node)  {
  (void)node;
  return 0;
}

static int ramfs_close(vfs_node_t *node) {
  (void)node;
  return 0;
}

static int ramfs_readdir(vfs_node_t *node, size_t index, vfs_dirent_t *dirent)
{
  if (!node || !dirent) return -1;

  const ramfs_file_t *dir = (ramfs_file_t *)node->private_data;

  if (!dir || !dir->is_dir) return -1;

  ramfs_file_t *child = dir->children;

  for (size_t i = 0; child != NULL; child = child->next, ++i) {
    if (i == index) {
      strncpy(dirent->name, child->name, sizeof(dirent->name));
      dirent->name[sizeof(dirent->name) - 1] = '\0';

      dirent->node = &child->node;
      return 0;
    }
  }

  return -1;
}

static vfs_node_t *ramfs_finddir(vfs_node_t *node, const char *name)
{
  const ramfs_file_t *dir = (ramfs_file_t *)node->private_data;

  if (!dir || !dir->is_dir) return NULL;

  ramfs_file_t *child = dir->children;
  while (child) {
    if (strcmp(child->name, name) == 0) {
      child->node.parent = node;
      return &child->node;
    }

    child = child->next;
  }

  return NULL;
}

static vfs_node_t *ramfs_create_node(vfs_node_t *parent_node, const char *name, bool is_dir, const void *content, size_t size)
{
  if (!parent_node || !name) return NULL;

  ramfs_file_t *parent = (ramfs_file_t *)parent_node->private_data;

  if (!parent || !parent->is_dir) return NULL;

  ramfs_file_t *file = kmalloc(sizeof(ramfs_file_t));
  if (!file) return NULL;
  memset(file, 0, sizeof(ramfs_file_t));

  strncpy(file->name, name, sizeof(file->name));
  file->name[sizeof(file->name) - 1] = '\0';
  file->is_dir = is_dir;
  file->parent = parent;

  if (!is_dir && content && size > 0) {
    file->data = kmalloc(size);

    if (!file->data) {
      kfree(file);
      return NULL;
    }

    memcpy(file->data, content, size);
    file->size = size;
  }

  ramfs_init_node_file(file);
  file->node.private_data = file;
  file->node.parent = parent_node;
  file->next = parent->children;
  parent->children = file;

  return &file->node;
}

int ramfs_rmf(vfs_node_t *parent_node, const char *name)
{
  if (!parent_node || !name) return -1;

  ramfs_file_t *parent = (ramfs_file_t *)parent_node->private_data;
  if (!parent || !parent->is_dir) return -1;

  ramfs_file_t *prev = NULL;
  ramfs_file_t *current = parent->children;

  while(current) {
    if (!current->is_dir && strcmp(current->name, name) == 0) {
      if (prev) prev->next = current->next;
      else parent->children = current->next;

      if (current->data) kfree(current->data);
      kfree(current);

      return 0;
    }

    prev = current;
    current = current->next;
  }

  return -1;
}

int ramfs_rmdir(vfs_node_t *parent_node, const char *name)
{
  if (!parent_node || !name) return -1;

  ramfs_file_t *parent = (ramfs_file_t *)parent_node->private_data;
  if (!parent || !parent->is_dir) return -1;

  ramfs_file_t *prev = NULL;
  ramfs_file_t *current = parent->children;

  while(current) {
    if (current->is_dir && strcmp(current->name, name) == 0) {

      if (current->children) return -2; // Not empty

      if (prev) prev->next = current->next;
      else parent->children = current->next;

      kfree(current);

      return 0;
    }

    prev = current;
    current = current->next;
  }
  
  return -1;
}
