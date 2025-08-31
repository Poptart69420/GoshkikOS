#include "timer.h"

void timer_handler(registers_t *reg)
{
  static int ticks = 0;
  ticks++;
  pic_send_eoi(0);
}

void init_timer(void)
{
  isr_register(0x20, timer_handler);
}
