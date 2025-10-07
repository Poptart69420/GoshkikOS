// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef _UTIME_H
#define _UTIME_H

#include <sys/types.h>

struct utimbuf {
  time_t actime;
  time_t modtime;
};

int utime(const char *path, const struct utimbuf *times);

#endif

_END_C_HEADER
