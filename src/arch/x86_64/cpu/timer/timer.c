#include <arch/x86_64/cpu/timer/timer.h>

void timer_handler(fault_frame_t *frame)
{
  (void)frame; // Suppress warning

  static int ticks = 0;
  ticks++;
  pic_send_eoi(0);

  (void)ticks; // Supress warning
}

void init_timer(void)
{
  isr_register(0x20, timer_handler);
  vterm_print("Timer...");
  kok();
}
