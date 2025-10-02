#ifndef VTERM_H_
#define VTERM_H_

#include <limine/limine.h>
#include <stddef.h>
#include <stdint.h>
#include <vterm/colors.h>
#include <vterm/font/font.h>

#define FB_HEIGHT 1080
#define FB_WIDTH 1920

void vterm_clear(uint32_t color);
void vterm_init(const struct limine_framebuffer *fb);
void vterm_putc(const char c);
void vterm_print(const char *str);
void vterm_set_fg_color(uint32_t color);
void vterm_set_term_color(uint32_t color);

#endif // VTERM_H_
