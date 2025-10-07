#include <arch/x86_64/ps2/keyboard/kbd.h>

static void keyboard_handler(fault_frame_t *frame) {
  uint8_t scancode = ps2_read();

  (void)scancode; // supress warning

  (void)frame;

  pic_send_eoi(1);
}

void init_keyboard(void) {
  isr_register(0x21, keyboard_handler);
  vterm_print("PS/2 Keyboard...");
  kok();
}
