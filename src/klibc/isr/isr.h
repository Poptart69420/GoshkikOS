#ifndef ISR_H_
#define ISR_H_

#include "../arch/x86_64/arch.h"
#include "../arch/x86_64/asm.h"
#include "../drivers/vterm/kerror.h"
#include "../drivers/vterm/kok.h"
#include "../drivers/vterm/vterm.h"
#include "../idt/idt.h"
#include "../include/hcf.h"
#include "../pic/pic.h"
#include <stddef.h>

// Handlers

#include "../timer/timer.h"

#define IDT_ENTRIES 256

typedef void (*handlers_t)(fault_frame_t *);

void kpanic(const char *error, fault_frame_t *fault);
void isr_register(int isr, handlers_t handler);
void isr_install(void);
void isr_handler(fault_frame_t *frame);

#endif // ISR_H_
