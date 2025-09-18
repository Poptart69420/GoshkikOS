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

  vterm_print("VFS:   Initalized");
}

int vfs_register_filesystem(filesystem_t *fs)
{
  vterm_print("VFS: Loading ");
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
