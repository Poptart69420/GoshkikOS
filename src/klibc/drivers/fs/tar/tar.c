#include "tar.h"

static inline uint64_t octal_to_int(const char *octal)
{
  uint64_t integer = 0;
  while (*octal) {
    integer *= 8;
    integer += (*octal) - '0';
    octal++;
  }

  return integer;
}

static int ensure_parent_dirs(const char *path)
{
  char tmp[256];
  strncpy(tmp, path, sizeof(tmp));
  tmp[sizeof(tmp) - 1] = '\0';

  size_t len = strlen(tmp);
  if (len > 1 && tmp[len - 1] == '/') tmp[len - 1] = '\0';

  char *p = tmp + 1;
  while (1) {
    char *slash = strchr(p, '/');
    if (!slash) break;
    *slash = '\0';

    const vfs_node_t *parent = vfs_lookup(tmp);
    if (!parent) {
      if (!vfs_create_dir(tmp)) {
        vterm_print("Failed to create parent directory: ");
        vterm_print(tmp);
        vterm_print("\n");
        return -1;
      }
    }

    *slash = '/';
    p = slash + 1;
  }

  return 0;
}

void mount_initrd(void)
{
  vfs_mount("ramfs", NULL, "/initrd");

  if (!vfs_lookup("/initrd")) {
    vterm_print("Initrd: Mount failed\n");
    hcf();
  }

  char *address = (char *)kernel->module->modules[0]->address;

  while (!memcmp(((tar_header *)address)->magic, "ustar", 5)) {
    const tar_header *current_file = (tar_header *)address;

    const char *name = current_file->name;
    if (name[0] == '.' && name[1] == '/') name += 2;
    if (strncmp(name, "initrd/", 7) == 0) name += 7;

    char *full_path = kmalloc(strlen(name) + strlen("/initrd/") + 1);
    strcpy(full_path, "/initrd/");
    strcat(full_path, name);

    uint64_t file_size = octal_to_int(current_file->file_size);
    const void *content = (const char *)current_file + 512;

    char parent_path[256];
    strncpy(parent_path, full_path, sizeof(parent_path));
    parent_path[sizeof(parent_path) - 1] = '\0';
    char *last = strrchr(parent_path, '/');
    if (last) {
      *last = '\0';
      if (ensure_parent_dirs(parent_path) < 0) {
        vterm_print("Initrd: Failed to create parent directories for: ");
        vterm_print(parent_path);
        vterm_print("\n");
        kfree(full_path);
        hcf();
      }
    }

    if (current_file->type == TAR_DIRTYPE) {
      full_path[strlen(full_path) - 1] = '\0';
      if (vfs_create_dir(full_path) == NULL) {
        vterm_print("Initrd: Failed to create directory\n");
        hcf();
      }
    } else {
      if (vfs_create_file(full_path, content, file_size) == NULL) {
        vterm_print("Initrd: Failed to create file\n");
        hcf();
      }
    }

    kfree(full_path);

    uint64_t blocks = (file_size + 511) / 512;
    address = (char *)current_file + (1 + blocks) * 512;
  }
}
