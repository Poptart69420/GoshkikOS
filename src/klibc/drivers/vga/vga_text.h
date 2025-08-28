#ifndef VGA_TEXT_H_
#define VGA_TEXT_H_

#include <stdint.h>
#include "../../include/io.h"

#define VGA_START 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_EXTENT (VGA_WIDTH * VGA_HEIGHT)

#define COLOR_BLACK   0
#define COLOR_BLUE    1
#define COLOR_GREEN   2
#define COLOR_CYAN    3
#define COLOR_RED     4
#define COLOR_PURPLE  5
#define COLOR_BROWN   6
#define COLOR_GRAY    7
#define COLOR_DGRAY   8
#define COLOR_LBLUE   9
#define COLOR_LGREEN  10
#define COLOR_LCYAN   11
#define COLOR_LRED    12
#define COLOR_LPURPLE 13
#define COLOR_YELLOW  14
#define COLOR_WHITE   15

#define CURSOR_COMMAND (uint16_t) 0x3D4
#define CURSOR_DATA    (uint16_t) 0x3D5

typedef struct __attribute__((packed))
{
  char character;
  char style;
} vga_char;

void clear_win(uint8_t fg_color, uint8_t bg_color);
uint8_t vga_color(const uint8_t fg_color, const uint8_t bg_color);
void set_cursor_pos(uint8_t x, uint8_t y);
void hide_cursor(void);
uint16_t get_cursor_pos(void);
void advance_cursor(void);
void reverse_cursor(void);
void show_cursor(void);
void scroll_line(void);
void vga_putchar(const char character, const uint8_t fg_color, const uint8_t bg_color);
void vga_putstr(const char *string, const uint8_t fg_color, const uint8_t bg_color);
void vga_puthex(uint8_t value, const uint8_t fg_color, const uint8_t bg_color);

#endif // VGA_TEXT_H_
