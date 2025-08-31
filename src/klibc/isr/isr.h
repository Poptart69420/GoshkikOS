#ifndef ISR_H_
#define ISR_H_

#include <stddef.h>
#include "../include/registers.h"
#include "../idt/idt.h"
#include "../drivers/vterm/vterm.h"
#include "../pic/pic.h"

// Handlers

#include "../timer/timer.h"

#define IDT_ENTRIES 256

typedef void (*handlers_t)(registers_t *);

void isr_register(int isr, handlers_t handler);
void isr_install(void);
void isr_handler(registers_t *reg);

#endif // ISR_H_
