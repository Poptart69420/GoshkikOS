#ifndef ISR_H_
#define ISR_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/asm/asm.h>
#include <arch/x86_64/asm/hcf.h>
#include <arch/x86_64/cpu/idt/idt.h>
#include <arch/x86_64/cpu/pic/pic.h>
#include <stddef.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

// Handlers

#include <arch/x86_64/cpu/timer/timer.h>

#define IDT_ENTRIES 256

typedef void (*handlers_t)(fault_frame_t *);

void kpanic(const char *error, fault_frame_t *fault);
void isr_register(int isr, handlers_t handler);
void isr_install(void);
void isr_handler(fault_frame_t *frame);

#endif // ISR_H_
