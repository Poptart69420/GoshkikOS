//FROM FILE COMES FROM TLIBC (tayoky 2025)
//SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef _CDEFS_H
#define _CDEFS_H

#ifdef __cpluscplus
#define _BEGIN_C_HEADER extern "C" {
#define _END_C_HEADER   }
#else
#define _BEGIN_C_HEADER
#define _END_C_HEADER
#endif

#endif

_END_C_HEADER
