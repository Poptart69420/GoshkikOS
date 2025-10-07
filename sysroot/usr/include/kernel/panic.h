#ifndef PANIC_H
#define PANIC_H
#include <kernel/arch.h>
#include <stdint.h>

void panic(const char *error, fault_frame *fault);

#ifndef NULL
#define NULL (void *)0
#endif

#endif