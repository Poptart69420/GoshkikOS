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

  if (!vfs_lookup("/initrd")) hcf();
}
