#ifndef TIMER_H_
#define TIMER_H_

#include "../drivers/vterm/kerror.h"
#include "../drivers/vterm/kok.h"
#include "../drivers/vterm/vterm.h"
#include "../include/registers.h"
#include "../isr/isr.h"
#include "../pic/pic.h"

void timer_handler(registers_t *reg);
void init_timer(void);

#endif // TIMER_H_
