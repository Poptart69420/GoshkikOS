#include <arch/x86_64/cpu/timer/timer.h>

void timer_handler(fault_frame_t *frame)
{
  (void)frame; // Suppress warning

  static int ticks = 0;
  ticks++;

  (void)ticks; // Supress warning
}
