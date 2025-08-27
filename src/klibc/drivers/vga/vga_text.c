#include "vga_text.h"

#include "vga_text.h"
#include "io.h"
#include <stdint.h>

volatile vga_char *TEXT_AREA = (vga_char*) VGA_START;

uint8_t vga_color(const uint8_t fg_color, const uint8_t bg_color)
  {
    return (bg_color << 4) | (fg_color & 0x0F);
  }

void clear_win(uint8_t fg_color, uint8_t bg_color)
  {
    uint8_t clear_color = vga_color(fg_color, bg_color);

    const vga_char clear_char =
      {
        .character = ' ',
        .style = clear_color
      };

    for (uint16_t i = 0; i < VGA_EXTENT; ++i) {
      TEXT_AREA[i] = clear_char;
    }
  }

void vga_putchar(const char character, const uint8_t fg_color, const uint8_t bg_color)
  {
    uint16_t pos = get_cursor_pos();

    if (character == '\n') {
      uint8_t current_row = (uint8_t) (pos / VGA_WIDTH);

      if (++current_row >= VGA_HEIGHT) {
        scroll_line();
      } else {
        set_cursor_pos(0, current_row);
      }
    } else if (character == '\b') {
      reverse_cursor();
      vga_putchar(' ', fg_color, bg_color);
      reverse_cursor();
    } else if (character == '\r') {
      uint8_t row = (uint8_t) (pos / VGA_WIDTH);
      set_cursor_pos(0, row);
    } else if (character == '\t') {
      for (uint8_t i = 0; i < 4; ++i) {
        vga_putchar(' ', fg_color, bg_color);
      }
      advance_cursor();
    } else {
      uint8_t style = vga_color(fg_color, bg_color);
      vga_char print = {
        .character = character,
        .style = style
      };

      TEXT_AREA[pos] = print;
      advance_cursor();
    }
  }

void vga_putstr(const char *string, const uint8_t fg_color, const uint8_t bg_color)
  {
    while (*string != '\0') {
      vga_putchar(*string++, fg_color, bg_color);
    }
  }

void set_cursor_pos(uint8_t x, uint8_t y)
  {
    uint16_t pos = (uint16_t) x + ((uint16_t) VGA_WIDTH * y);

    if (pos >= VGA_EXTENT) {
        pos = VGA_EXTENT - 1;
    }

    outb(CURSOR_COMMAND, 0x0F);
    outb(CURSOR_DATA, (uint8_t) (pos & 0xFF));

    outb(CURSOR_COMMAND, 0x0E);
    outb(CURSOR_DATA, (uint8_t) ((pos >> 8) & 0xFF));
  }

uint16_t get_cursor_pos(void)
  {
    uint16_t pos = 0;

    outb(CURSOR_COMMAND, 0x0F);
    pos |= inb(CURSOR_DATA);

    outb(CURSOR_COMMAND, 0x0E);
    pos |= inb(CURSOR_DATA) << 8;

    return pos;
  }

void advance_cursor(void)
  {
    uint16_t pos = get_cursor_pos();
    pos++;

    outb(CURSOR_COMMAND, 0x0F);
    outb(CURSOR_DATA, (uint8_t) (pos & 0xFF));

    outb(CURSOR_COMMAND, 0x0E);
    outb(CURSOR_DATA, (uint8_t) ((pos >> 8) & 0xFF));
  }

void reverse_cursor(void)
  {
    unsigned short pos = get_cursor_pos();
    pos--;

    outb(CURSOR_COMMAND, 0x0F);
    outb(CURSOR_DATA, (unsigned char) (pos & 0xFF));

    outb(CURSOR_COMMAND, 0x0E);
    outb(CURSOR_DATA, (unsigned char) ((pos >> 8) & 0xFF));
  }

void hide_cursor(void)
  {
    outb(CURSOR_COMMAND, 0x0A);
    outb(CURSOR_DATA, 0x20);
  }

void show_cursor(void)
  {
    uint8_t current;

    outb(CURSOR_COMMAND, 0x0A);
    current = inb(CURSOR_DATA);
    outb(CURSOR_DATA, current & 0xC0);

    outb(CURSOR_COMMAND, 0x0B);
    current = inb(CURSOR_DATA);
    outb(CURSOR_DATA, current & 0xE0);
  }

void scroll_line(void)
{
  for (uint16_t i = 1; i < VGA_HEIGHT; ++i) {
    for (uint16_t j = 0; j < VGA_WIDTH; ++j) {
      uint16_t to_pos = j + ((i - 1) * VGA_WIDTH);
      uint16_t from_pos = j + (i * VGA_WIDTH);

      TEXT_AREA[to_pos] = TEXT_AREA[from_pos];

    }
  }

  uint16_t i = VGA_HEIGHT - 1;
  for (uint16_t j = 0; j < VGA_WIDTH; ++j) {
    uint16_t pos = j + (i * VGA_WIDTH);

    vga_char current = TEXT_AREA[pos];
    vga_char clear = {
      .character = ' ',
      .style = current.style
    };

    TEXT_AREA[pos] = clear;
  }

  set_cursor_pos(0, VGA_HEIGHT - 1);
}

void vga_puthex(uint8_t value, const uint8_t fg_color, const uint8_t bg_color)
{
  const char *hex_digits = "0123456789ABCDEF";

  vga_putchar('0', fg_color, bg_color);
  vga_putchar('x', fg_color, bg_color);
  vga_putchar(hex_digits[(value >> 4) & 0xF], fg_color, bg_color);
  vga_putchar(hex_digits[value & 0xF], fg_color, bg_color);
}
