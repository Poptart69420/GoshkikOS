#ifndef IRQ_H_
#define IRQ_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/asm/asm.h>
#include <arch/x86_64/asm/hcf.h>
#include <arch/x86_64/cpu/idt/idt.h>
#include <arch/x86_64/cpu/pic/pic.h>
#include <arch/x86_64/cpu/timer/timer.h>
#include <arch/x86_64/ps2/keyboard/kbd.h>
#include <klibc/printf.h>
#include <scheduling/scheduler.h>
#include <stddef.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

void irq_handler(context_t *context);

#endif // IRQ_H_
