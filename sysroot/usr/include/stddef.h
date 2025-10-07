// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef STDDEF_H
#define STDDEF_H

#include <sys/types.h>

#ifndef NULL
#define NULL (void *)0
#endif

#ifndef __WCHAR_TYPE__
#define __WCHAR_TYPE__ unsigned int
#endif

typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __WCHAR_TYPE__ wchar_t;

#define offsetof(struct_type, member) (size_t)&((struct struct_type *)0)->member

#endif

_END_C_HEADER
