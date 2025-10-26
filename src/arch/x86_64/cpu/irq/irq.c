#include <arch/x86_64/cpu/irq/irq.h>

void irq_handler(fault_frame_t *frame)
{
  if (!frame)
    return;

  uint32_t irq_num = frame->int_no - 32;
  if (irq_num > 15)
    return;

  pic_send_eoi(irq_num);

  switch (irq_num)
  {
  case 0:
    kprintf("Timer\n");
    scheduler_tick(frame);
    timer_handler(frame);
    break;
  case 1:
    keyboard_handler(frame);
    break;
  default:
    break;
  }
}
