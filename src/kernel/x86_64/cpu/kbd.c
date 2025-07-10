#include "kbd.h"
#include "../vga_text.h"
#include "../io.h"
#include <stdint.h>

static int expecting_break = 0;

static uint8_t ps2_translate_keycode(uint64_t scancode)
{
  switch(scancode)
    {
    case 0x76: return KEY_ESC;
    case 0x16: return KEY_1;
    case 0x1E: return KEY_2;
    case 0x26: return KEY_3;
    case 0x25: return KEY_4;
    case 0x2E: return KEY_5;
    case 0x36: return KEY_6;
    case 0x3D: return KEY_7;
    case 0x3E: return KEY_8;
    case 0x46: return KEY_9;
    case 0x45: return KEY_0;
    case 0x4E: return KEY_MINUS;
    case 0x55: return KEY_EQUAL;
    case 0x66: return KEY_BACKSPACE;
    case 0x0D: return KEY_TAB;
    case 0x15: return KEY_Q;
    case 0x1D: return KEY_W;
    case 0x24: return KEY_E;
    case 0x2D: return KEY_R;
    case 0x2C: return KEY_T;
    case 0x35: return KEY_Y;
    case 0x3C: return KEY_U;
    case 0x43: return KEY_I;
    case 0x44: return KEY_O;
    case 0x4D: return KEY_P;
    case 0x54: return KEY_LBRACE;
    case 0x5B: return KEY_RBRACE;
    case 0x5A: return KEY_ENTER;
    case 0x14: return KEY_LCTRL;
    case 0x1C: return KEY_A;
    case 0x1B: return KEY_S;
    case 0x23: return KEY_D;
    case 0x2B: return KEY_F;
    case 0x34: return KEY_G;
    case 0x33: return KEY_H;
    case 0x3B: return KEY_J;
    case 0x42: return KEY_K;
    case 0x4B: return KEY_L;
    default: return KEY_NULL;
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
  uint64_t timeout = PS2_TIMEOUT;
  while (--timeout) {
    if (!(inb(PS2_COMMAND) & (1 << 1))) return 0;
  }
  return 1;
}

static int ps2_wait_output(void)
{
  uint64_t timeout = PS2_TIMEOUT;
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
  ps2_flush_output(PS2_TIMEOUT);

}

void ps2_setup(void)
{
  outb(PS2_COMMAND, PS2_DISABLE_P1);
  outb(PS2_COMMAND, PS2_DISABLE_P2);

  ps2_flush_output(PACKETS_IN_PIPE);

  uint8_t status;

  ps2_wait_input();

  status = inb(PS2_DATA);

  status &= ~0x47;
  status |= 0x01;

  ps2_wait_input();
  outb(PS2_COMMAND, PS2_DATA);
  ps2_wait_input();
  outb(PS2_DATA, status);

  outb(PS2_COMMAND, PS2_ENABLE_P1);
  outb(PS2_COMMAND, PS2_ENABLE_P2);
  set_kboard_scancode();
}

void ps2_keyboard_handler(void)
{
  uint8_t code = inb(PS2_DATA);

  if (code == 0xFA || code == 0xFE) {
    ps2_flush_output(PS2_TIMEOUT);
    return;
  }

  if (code == 0xF0) {
    expecting_break = 1;
    return;
  }

  if (expecting_break) {
    putstr("\nKey released: ", COLOR_YELLOW, COLOR_BLACK);
    expecting_break = 0;
  } else {
    putstr("\nKey pressed: ", COLOR_GREEN, COLOR_BLACK);
  }

  puthex(code, COLOR_BLUE, COLOR_BLACK);
  uint8_t translated = ps2_translate_keycode(code);
  putstr(" => ", COLOR_CYAN, COLOR_BLACK);
  puthex(translated, COLOR_RED, COLOR_BLACK);
}
