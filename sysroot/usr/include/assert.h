//FROM FILE COMES FROM TLIBC (tayoky 2025)
//SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef ASSERT_H
#define ASSERT_H

#ifdef NDEBUG
#define assert(cond)
#else
void __assert(const char *expr,const char *file,long line,const char *func);
#define assert(cond) ((cond) ? (void)(0) : __assert(#cond,__FILE__,__LINE__,__func__))
#endif


#endif
_END_C_HEADER
