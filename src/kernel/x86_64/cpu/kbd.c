#include "kbd.h"
#include "../vga_text.h"
#include "../io.h"
#include <stdint.h>

static uint8_t ps2_translate_keycode(uint64_t scancode)
{
  switch(scancode)
    {
    case 0x01:
      return KEY_ESC;
    default:
      return KEY_NULL;
    }
}

static void ps2_flush_output(uint16_t timeout)
  {
    while (inb(PS2_COMMAND) & 1 && timeout > 0) {
      timeout--;
      inb(PS2_DATA);
    }
  }

static int ps2_wait_input(void)
  {
    uint64_t timeout = 1000;
    while (--timeout) {
      if (!(inb(PS2_COMMAND) & (1 << 1))) return 0;
    }
    return 1;
  }

static int ps2_wait_output(void)
  {
    uint64_t timeout = 1000;
    while (--timeout) {
      if (inb(PS2_COMMAND) & (1 << 0)) return 0;
    }
    return 1;
  }

static void set_kboard_scancode(void)
{
  ps2_wait_input();
  outb(PS2_DATA, 0xF0);
  ps2_wait_output();
  if (inb(PS2_DATA) != 0xFA) {
    putstr("Failed to send 0xF0\n", COLOR_RED, COLOR_BLACK);
    return;
  }

  ps2_wait_input();
  outb(PS2_DATA, 0x02);
  ps2_wait_output();
  if (inb(PS2_DATA) != 0xFA) {
    putstr("Failed to send scancode set\n", COLOR_RED, COLOR_BLACK);
  }
}

void ps2_setup(void)
{
    outb(PS2_COMMAND, PS2_DISABLE_P1);
    outb(PS2_COMMAND, PS2_DISABLE_P2);

    ps2_flush_output(PACKETS_IN_PIPE);

    uint8_t status;

    ps2_wait_input();

    status = inb(PS2_DATA);

    status &= ~0x30;
    status |= 0x01;

    ps2_wait_input();
    outb(PS2_COMMAND, PS2_DATA);
    ps2_wait_input();
    outb(PS2_DATA, status);

    outb(PS2_COMMAND, PS2_ENABLE_P1);
    outb(PS2_COMMAND, PS2_ENABLE_P2);
}

void ps2_keyboard_handler(void)
{
  uint64_t scancode = inb(PS2_DATA);
  putstr("\nScancode before translation: ", COLOR_YELLOW, COLOR_BLACK);
  puthex(scancode, COLOR_GREEN, COLOR_BLACK);

  uint8_t translated_scancode = ps2_translate_keycode(scancode);
  putstr("\nScancode after translation: ", COLOR_BLUE, COLOR_BLACK);
  puthex(translated_scancode, COLOR_RED, COLOR_BLACK);
}
