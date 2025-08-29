#ifndef VTERM_H_
#define VTERM_H_

#include <stdint.h>
#include <stddef.h>
#include "../../../limine.h"
#include "font/font.h"

void vterm_clear(uint32_t color);
void vterm_init(const struct limine_framebuffer *fb);
void vterm_putc(const char c);
void vterm_print(const char *str);

#endif // VTERM_H_
