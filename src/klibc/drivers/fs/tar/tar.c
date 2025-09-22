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

    char *full_path = kmalloc(strlen(current_file->name) + strlen("/initrd/") + 1);
    strcpy(full_path, "/initrd/");
    strcat(full_path, current_file->name);

    uint64_t file_size = octal_to_int(current_file->file_size);
    const void *content = (const char *)current_file + 512;

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
