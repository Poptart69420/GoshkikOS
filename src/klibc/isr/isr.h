#ifndef ISR_H_
#define ISR_H_

#include <stddef.h>
#include "../include/registers.h"
#include "../idt/idt.h"
#include "../drivers/vterm/vterm.h"

typedef void (*handlers_t)(registers_t *);

void isr_install(void);
void isr_handler(registers_t *reg);

#endif // ISR_H_
