#ifndef SYSCALL_H_
#define SYSCALL_H_

#include <stdint.h>

static inline long sys_call(long n, long a1, long a2, long a3) {
    long ret;
    asm volatile("syscall"
                 : "=a"(ret)
                 : "a"(n), "D"(a1), "S"(a2), "d"(a3)
                 : "rcx", "r11", "memory");
    return ret;
}

#endif // SYSCALL_H_
