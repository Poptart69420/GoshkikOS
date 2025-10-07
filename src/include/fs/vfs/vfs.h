#ifndef VFS_H_
#define VFS_H_

#include <arch/x86_64/asm/hcf.h>
#include <klibc/kmem/hashtable.h>
#include <klibc/kmem/kheap.h>
#include <klibc/kmem/slab.h>
#include <klibc/kmem/string.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

#define VFS_REG 0
#define VFS_DIR 1

#define VFS_FLAG_READ 1
#define VFS_FLAG_WRITE 2
#define VFS_FLAG_LOOKUP_CREATE_PARENTS (1 << 0)

#endif // VFS_H_
