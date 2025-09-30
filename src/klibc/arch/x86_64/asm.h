#ifndef ASM_H
#define ASM_H

#include <stdint.h>

#define disable_interrupt() __asm__ volatile("cli")
#define enable_interrupt() __asm__ volatile("sti")
#define halt() while (1)

static inline int have_interrupt() {
    uintptr_t flags;
    __asm__ volatile("pushf\n"
                     "pop %0"
                     : "=rm"(flags));

    return (flags & (1 << 9));
}

#endif // ASM_H_
