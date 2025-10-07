// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <sys/types.h>

#define PROT_READ 0x1  /* page can be read */
#define PROT_WRITE 0x2 /* page can be written */
#define PROT_EXEC 0x4  /* page can be executed */
#define PROT_NONE 0x0  /* page can not be accessed */

#define MAP_SHARED 0x01    /* modification are syncronise to the file/device */
#define MAP_PRIVATE 0x02   /* create a copy on write page */
#define MAP_FIXED 0x10     /* interpret addr exactly */
#define MAP_ANONYMOUS 0x20 /* don't use a file */

#define map_failed (void *)-1
#define MAP_FAILED (void *)-1

void *mmap(void *addr, size_t length, int prot, int flags, int fd,
           off_t offset);
int munmap(void *addr, size_t length);
int mprotect(void *addr, size_t size, int prot);
int shm_open(const char *name, int oflag, mode_t mode);
int shm_unlink(const char *name);

#endif

_END_C_HEADER
