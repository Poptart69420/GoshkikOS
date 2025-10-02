#ifndef TIMER_H_
#define TIMER_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/cpu/isr/isr.h>
#include <arch/x86_64/cpu/pic/pic.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

void timer_handler(fault_frame_t *frame);
void init_timer(void);

#endif // TIMER_H_
