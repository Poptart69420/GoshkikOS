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

void hide_cursor(void)
{
    outb(CURSOR_COMMAND, 0x0A);
    outb(CURSOR_DATA, 0x20);
}
