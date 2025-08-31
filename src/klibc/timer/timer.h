#ifndef TIMER_H_
#define TIMER_H_

#include "../include/registers.h"
#include "../drivers/vterm/vterm.h"
#include "../pic/pic.h"
#include "../isr/isr.h"

void timer_handler(registers_t *reg);
void init_timer(void);

#endif // TIMER_H_
