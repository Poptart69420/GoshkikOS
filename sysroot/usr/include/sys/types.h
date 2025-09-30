//FROM FILE COMES FROM TLIBC (tayoky 2025)
//SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef SYS_TYPES_H
#define SYS_TYPES_H


typedef long pid_t;
typedef long uid_t;
typedef long gid_t;
typedef long mode_t;
typedef long nlink_t ;

typedef long time_t;
typedef unsigned long useconds_t;
typedef long suseconds_t;
typedef long off_t;

//stupid bloat for posix API
typedef long dev_t;
typedef long ino_t;
typedef long blksize_t;
typedef long blkcnt_t;

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ unsigned long
#endif
#ifndef __PTRDIFF_TYPE__
#define __PTRDIFF_TYPE__ signed long
#endif

typedef __SIZE_TYPE__ size_t;
typedef __PTRDIFF_TYPE__ ssize_t;
#endif

_END_C_HEADER
