#include <arch/x86_64/ps2/keyboard/kbd.h>

void keyboard_handler(context_t *context)
{
  uint8_t scancode = ps2_read();

  (void)scancode; // supress warning

  (void)context;
}
