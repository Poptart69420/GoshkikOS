//FROM FILE COMES FROM TLIBC (tayoky 2025)
//SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef LIMITS_H
#define LIMITS_H


//intergers limits

#ifdef __CHAR_BIT__
#define CHAR_BIT __CHAR_BIT__
#else
#define CHAR_BIT 8
#endif

#ifdef __SCHAR_MAX__
#define SCHAR_MAX __SCHAR_MAX__
#else
#define SCHAR_MAX 127
#endif

#define SCHAR_MIN (-SCHAR_MAX -1)
#define UCHAR_MAX (SCHAR_MAX * 2 + 1)

#ifdef __SHRT_MAX__
#define SHRT_MAX __SHRT_MAX__
#else
#define SHRT_MAX 32767
#endif

#define SHRT_MIN (-SHRT_MAX -1)
#define USHRT_MAX (SHRT_MAX * 2UL + 1)

#ifdef __INT_MAX__
#define INT_MAX __INT_MAX__
#else
#define INT_MAX 2147483647
#endif

#define INT_MIN (-INT_MAX -1)
#define UINT_MAX (INT_MAX * 2 + 1)

#ifdef __LONG_MAX__
#define LONG_MAX __LONG_MAX__
#elif defined(__i386__)
#define LONG_MAX INT_MAX
#else
#define LONG_MAX 9223372036854775807UL
#endif

#define LONG_MIN (-LONG_MAX -1)
#define ULONG_MAX (LONG_MAX * 2UL + 1)

#ifdef __LONG_LONG_MAX__
#define LLONG_MAX __LONG_LONG_MAX__
#else
#define LLONG_MAX LONG_MAX
#endif

#define LLONG_MIN (-LLONG_MAX -1)
#define ULLONG_MAX (LLONG_MAX * 2UL + 1)

#ifdef __PTRDIFF_MAX__
#define SSIZE_MAX __PTRDIFF_MAX__
#else
#define SSIZE_MAX LONG_MAX
#endif

#define SSIZE_MIN (-SSIZE_MAX -1)

#define PATH_MAX 256
#define ATEXIT_MAX 63
#define PAGESIZE 4096
#define PAGE_SIZE PAGESIZE

#define MB_CUR_MAX 4
#define SYMLOOP_MAX 8 //os specific

#endif

_END_C_HEADER
