#ifndef TIMER_H_
#define TIMER_H_

#include "../arch/x86_64/arch.h"
#include "../drivers/vterm/kerror.h"
#include "../drivers/vterm/kok.h"
#include "../drivers/vterm/vterm.h"
#include "../isr/isr.h"
#include "../pic/pic.h"

void timer_handler(fault_frame_t *frame);
void init_timer(void);

#endif // TIMER_H_
