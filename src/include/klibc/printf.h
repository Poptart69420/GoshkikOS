#ifndef PRINTF_H_
#define PRINTF_H_

#include <arch/x86_64/serial/serial.h>
#include <klibc/kmem/string.h>
#include <stddef.h>
#include <stdint.h>
#include <vterm/vterm.h>

#define DIGITS_UPPERCASE "0123456789ABCDEF"
#define DIGITS_LOWERCASE "0123456789abcdef"

int kprintf(const char *format, ...);

#endif // PRINTF_H_
