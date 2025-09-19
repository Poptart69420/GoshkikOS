#ifndef RAMFS_H_
#define RAMFS_H_

#include "../vfs_structs.h"
#include "../vfs.h"
#include "../../../mem/kheap.h"

typedef struct ramfs_file
{
  char name[256];
  bool is_dir;
  uint8_t *data;
  size_t size;
  struct ramfs_file *parent;
  struct ramfs_file *children;
  struct ramfs_file *next;
} ramfs_file_t;

extern filesystem_t fs_ramfs;

#endif // RAMFS_H_
