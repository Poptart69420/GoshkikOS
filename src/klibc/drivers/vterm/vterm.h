#ifndef VTERM_H_
#define VTERM_H_

#include "../../../limine.h"
#include "font/font.h"
#include <stddef.h>
#include <stdint.h>

void vterm_clear(uint32_t color);
void vterm_init(const struct limine_framebuffer *fb);
void vterm_putc(const char c);
void vterm_print(const char *str);

#endif // VTERM_H_
