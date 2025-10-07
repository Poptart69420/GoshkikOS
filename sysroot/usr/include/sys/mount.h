// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef _SYS_MOUNT_H
#define _SYS_MOUNT_H

int mount(const char *source, const char *target, const char *filesystemtype,
          unsigned long mountflags, const void *data);

int unmount(const char *dir, int flags);
int umount(const char *target);

#endif
_END_C_HEADER
