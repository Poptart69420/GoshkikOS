#include "kbd.h"
#include "../vga_text.h"
#include "../io.h"
#include <stdint.h>
#include <stdbool.h>
#include <memory.h>

static uint8_t kbd_scancode[PS2_BUFFER_MAX];
static uint8_t kbd_scancode_pos = 0;
static bool ignore_input = false;

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
    case 0x4C: return KEY_SEMICOLON;
    case 0x52: return KEY_APOSTROPHE;
    case 0x0E: return KEY_GRAVE;
    case 0x12: return KEY_LSHIFT;
    case 0x5D: return KEY_BACKSLASH;
    case 0x1A: return KEY_Z;
    case 0x22: return KEY_X;
    case 0x21: return KEY_C;
    case 0x2A: return KEY_V;
    case 0x32: return KEY_B;
    case 0x31: return KEY_N;
    case 0x3A: return KEY_M;
    case 0x41: return KEY_COMMA;
    case 0x49: return KEY_PERIOD;
    case 0x4A: return KEY_SLASH;
    case 0x59: return KEY_RSHIFT;
    case 0x7C: return KEY_NUM_ASTERISK;
    case 0x11: return KEY_LALT;
    case 0x29: return KEY_SPACE;
    case 0x58: return KEY_CAPSLOCK;
    case 0x05: return KEY_F1;
    case 0x06: return KEY_F2;
    case 0x04: return KEY_F3;
    case 0x0C: return KEY_F4;
    case 0x03: return KEY_F5;
    case 0x0B: return KEY_F6;
    case 0x83: return KEY_F7;
    case 0x0A: return KEY_F8;
    case 0x01: return KEY_F9;
    case 0x09: return KEY_F10;
    case 0x77: return KEY_NUMLOCK;
    case 0x7E: return KEY_SCROLLLOCK;
    case 0x6C: return KEY_NUM_7;
    case 0x75: return KEY_NUM_8;
    case 0x7D: return KEY_NUM_9;
    case 0x7B: return KEY_NUM_MINUS;
    case 0x6B: return KEY_NUM_4;
    case 0x73: return KEY_NUM_5;
    case 0x74: return KEY_NUM_6;
    case 0x79: return KEY_NUM_PLUS;
    case 0x69: return KEY_NUM_1;
    case 0x72: return KEY_NUM_2;
    case 0x7A: return KEY_NUM_3;
    case 0x70: return KEY_NUM_0;
    case 0x71: return KEY_NUM_PERIOD;
    case 0x78: return KEY_F11;
    case 0x07: return KEY_F12;

    case 0xE05A: return KEY_NUM_ENTER;
    case 0xE014: return KEY_RCTRL;
    case 0xE04A: return KEY_NUM_SLASH;

    case 0xE012E07C: return KEY_SYSRQ;
    case 0xE011: return KEY_RALT;
    case 0xE06C: return KEY_HOME;
    case 0xE075: return KEY_UP;
    case 0xE07D: return KEY_PAGEUP;
    case 0xE06B: return KEY_LEFT;
    case 0xE074: return KEY_RIGHT;
    case 0xE069: return KEY_END;
    case 0xE072: return KEY_DOWN;
    case 0xE07A: return KEY_PAGEDOWN;
    case 0xE070: return KEY_INSERT;
    case 0xE071: return KEY_DELETE;
    case 0xE023: return KEY_MUTE;
    case 0xE021: return KEY_VOL_DOWN;
    case 0xE032: return KEY_VOL_UP;
    case 0xE037: return KEY_POWER;
    case 0xE11477E1F014F077: return KEY_PAUSE;

    case 0xF076: return 0x80 | KEY_ESC;
    case 0xF016: return 0x80 | KEY_1;
    case 0xF01E: return 0x80 | KEY_2;
    case 0xF026: return 0x80 | KEY_3;
    case 0xF025: return 0x80 | KEY_4;
    case 0xF02E: return 0x80 | KEY_5;
    case 0xF036: return 0x80 | KEY_6;
    case 0xF03D: return 0x80 | KEY_7;
    case 0xF03E: return 0x80 | KEY_8;
    case 0xF046: return 0x80 | KEY_9;
    case 0xF045: return 0x80 | KEY_0;
    case 0xF04E: return 0x80 | KEY_MINUS;
    case 0xF055: return 0x80 | KEY_EQUAL;
    case 0xF066: return 0x80 | KEY_BACKSPACE;
    case 0xF00D: return 0x80 | KEY_TAB;
    case 0xF015: return 0x80 | KEY_Q;
    case 0xF01D: return 0x80 | KEY_W;
    case 0xF024: return 0x80 | KEY_E;
    case 0xF02D: return 0x80 | KEY_R;
    case 0xF02C: return 0x80 | KEY_T;
    case 0xF035: return 0x80 | KEY_Y;
    case 0xF03C: return 0x80 | KEY_U;
    case 0xF043: return 0x80 | KEY_I;
    case 0xF044: return 0x80 | KEY_O;
    case 0xF04D: return 0x80 | KEY_P;
    case 0xF054: return 0x80 | KEY_LBRACE;
    case 0xF05B: return 0x80 | KEY_RBRACE;
    case 0xF05A: return 0x80 | KEY_ENTER;
    case 0xF014: return 0x80 | KEY_LCTRL;
    case 0xF01C: return 0x80 | KEY_A;
    case 0xF01B: return 0x80 | KEY_S;
    case 0xF023: return 0x80 | KEY_D;
    case 0xF02B: return 0x80 | KEY_F;
    case 0xF034: return 0x80 | KEY_G;
    case 0xF033: return 0x80 | KEY_H;
    case 0xF03B: return 0x80 | KEY_J;
    case 0xF042: return 0x80 | KEY_K;
    case 0xF04B: return 0x80 | KEY_L;
    case 0xF04C: return 0x80 | KEY_SEMICOLON;
    case 0xF052: return 0x80 | KEY_APOSTROPHE;
    case 0xF00E: return 0x80 | KEY_GRAVE;
    case 0xF012: return 0x80 | KEY_LSHIFT;
    case 0xF05D: return 0x80 | KEY_BACKSLASH;
    case 0xF01A: return 0x80 | KEY_Z;
    case 0xF022: return 0x80 | KEY_X;
    case 0xF021: return 0x80 | KEY_C;
    case 0xF02A: return 0x80 | KEY_V;
    case 0xF032: return 0x80 | KEY_B;
    case 0xF031: return 0x80 | KEY_N;
    case 0xF03A: return 0x80 | KEY_M;
    case 0xF041: return 0x80 | KEY_COMMA;
    case 0xF049: return 0x80 | KEY_PERIOD;

    default: return KEY_NULL;
    }
}

static void ps2_clear_scancode(void)
  {
    memset(&kbd_scancode[0], 0x00, 8);
    kbd_scancode_pos = 0;
  }

static bool ps2_next_byte(const uint8_t* scancode, size_t bytes)
{
  switch(bytes) {
  case 1:
    if (scancode[0] == 0xE0) return true;
    if (scancode[0] == 0xF0) return true;
    if (scancode[0] == 0xE1) return true; // pause
    break;
  case 2:
    if (scancode[1] == 0xF0) return true;
    if (scancode[0] == 0xE0 && scancode[1] == 0x12) return true; // sysrq/print screen
    if (scancode[0] == 0xE1 && scancode[1] == 0x14) return true; // pause
    break;
  case 3:
    if (scancode[2] == 0x77) return true; // pause
    if (scancode[2] == 0x7C) return true; // sysrq/print screen
    if (scancode[2] == 0xE0) return true; // sysrq/print screen
    break;
  case 4:
    if (scancode[3] == 0xE0) return true; // sysrq/print screen
    if (scancode[3] == 0xE1) return true; // pause
    break;
  case 5:
    if (scancode[4] == 0xF0) return true; // sysrq/print screen || pause
    break;
  case 6:
    if (scancode[5] == 0x14) return true; // pause
    break;
  case 7:
    if (scancode[6] == 0xF0) return true; // pause
    break;
  default:
    break; // shouldn't be needed
  }
  return false;
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
  ignore_input = true;

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

  ignore_input = false;
  ps2_flush_output(PS2_TIMEOUT);
  inb(PS2_DATA);
}

void ps2_keyboard_handler(void)
{
  if (ignore_input) {
    ps2_flush_output(PS2_DATA);
    return;
  }

  uint8_t code = inb(PS2_DATA);

  if (code == 0xFA || code == 0xFE) {
    ps2_flush_output(PS2_TIMEOUT);
    return;
  }

  if (kbd_scancode_pos < PS2_BUFFER_MAX) {
    kbd_scancode[kbd_scancode_pos++] = code;
  } else {
    kbd_scancode_pos = 0;
    return;
  }

  if (ps2_next_byte(kbd_scancode, kbd_scancode_pos)) {
    return;
  }

  uint64_t full_scancode = 0;
  for (uint8_t i = 0; i < kbd_scancode_pos; ++i) {
    full_scancode <<= 8;
    full_scancode |= kbd_scancode[i];
  }

  putstr("\nKey pressed: ", COLOR_GREEN, COLOR_BLACK);

  puthex(full_scancode, COLOR_BLUE, COLOR_BLACK);
  uint8_t translated = ps2_translate_keycode(full_scancode);
  putstr(" => ", COLOR_CYAN, COLOR_BLACK);
  puthex(translated, COLOR_RED, COLOR_BLACK);
  ps2_clear_scancode();
}
