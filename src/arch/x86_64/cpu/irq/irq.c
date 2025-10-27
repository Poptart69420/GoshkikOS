#include <arch/x86_64/cpu/irq/irq.h>

void irq_handler(context_t *context)
{
  if (!context)
    return;

  uint32_t irq_num = context->int_no - 32;
  if (irq_num > 15)
    return;

  pic_send_eoi(irq_num);

  switch (irq_num)
  {
  case 0:
    scheduler_tick(context);
    timer_handler(context);
    break;
  case 1:
    keyboard_handler(context);
    break;
  default:
    break;
  }
}
