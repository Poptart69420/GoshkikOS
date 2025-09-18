#ifndef VFS_H_
#define VFS_H_

#include "vfs_structs.h"
#include "../vterm/vterm.h"

#define MAX_FILESYSTEMS 8
#define MAX_MOUNTS 16

void init_vfs(void);

#endif // VFS_H_
