#include "kbd.h"

static void keyboard_handler(registers_t *regs)
{
  uint8_t scancode = ps2_read();

  vterm_print("Key Press\n");

  pic_send_eoi(1);
}

void init_keyboard(void)
{
  isr_register(0x21, keyboard_handler);
}
