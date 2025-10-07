// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef _UCONTEXT
#define _UCONTEXT

#include <stdint.h>
#include <sys/signal.h>

typedef uintptr_t mcontext_t[64];
typedef struct __ucontext {
  struct __ucontext *uc_link;
  sigset_t uc_sigmask;
  stack_t uc_stack;
  mcontext_t uc_mcontext;
} ucontext_t;

#endif
_END_C_HEADER
