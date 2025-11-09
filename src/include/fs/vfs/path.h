#ifndef _PATH_H_
#define _PATH_H_

#include <fs/vfs/vfs.h>
#include <scheduling/process.h>

// Limits
#define NAMEI_SYMLINKS_MAX 16
#define NAMEI_PATHBUF_SIZE 4096

// Flags
#define NAMEI_FOLLOW 0x01   // Follow symlinks
#define NAMEI_NOFOLLOW 0x00 // Don't follow symlinks

// Structs
typedef struct
{
  hashtable_t table; // Table of directory vnode pointers
  spinlock_t lock;   // Name cache spinlock
} namecache_t;

struct namecache_key
{
  uintptr_t dvp_ptr;       // Directory vnode pointer (value)
  char name[NAME_MAX + 1]; // Directory vnode name (key)
};

#endif // _PATH_H_
