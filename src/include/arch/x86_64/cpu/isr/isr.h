#ifndef ISR_H_
#define ISR_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/asm/asm.h>
#include <arch/x86_64/asm/hcf.h>
#include <arch/x86_64/cpu/idt/idt.h>
#include <arch/x86_64/cpu/pic/pic.h>
#include <klibc/printf.h>
#include <stddef.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

void isr_handler(context_t *context);

#endif // ISR_H_
