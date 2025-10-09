#ifndef ASSERT_H
#define ASSERT_H

#include <arch/x86_64/asm/hcf.h>
#include <vterm/kerror.h>

__attribute__((noreturn)) void __assert(const char *expr, const char *file, int line, const char *func);

#define assert(expr) \
  ((expr) ? (void)0 : __assert(#expr, __FILE__, __LINE__, __func__))

#endif /* ASSERT_H */
