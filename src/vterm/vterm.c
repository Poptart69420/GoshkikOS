#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

static volatile uint32_t *framebuffer;
static uint64_t pitch;
static uint64_t width;
static uint64_t height;

static size_t cursor_x = 0;
static size_t cursor_y = 0;
static uint32_t term_color; // black
static uint32_t fg_color;   // green

void vterm_set_fg_color(uint32_t color)
{
  fg_color = color;
}

void vterm_set_term_color(uint32_t color)
{
  term_color = color;
}

void vterm_clear(uint32_t color)
{
  for (size_t y = 0; y < height; ++y)
  {
    for (size_t x = 0; x < width; ++x)
    {
      framebuffer[y * (pitch / 4) + x] = color;
    }
  }
  cursor_x = 0;
  cursor_y = 0;
}

void vterm_init(const struct limine_framebuffer *fb)
{
  framebuffer = fb->address;
  pitch = fb->pitch;
  width = fb->width;
  height = fb->height;

  vterm_set_fg_color(COLOR_WHITE);
  vterm_set_term_color(COLOR_BLACK);

  vterm_clear(term_color);
}

void vterm_putc(const char c)
{
  if (c == '\n')
  {
    cursor_x = 0;
    cursor_y += 16;
    return;
  }

  if (cursor_x + 8 > width)
  {
    cursor_x = 0;
    cursor_y += 16;
  }

  if (cursor_y + 8 > height)
  {
    vterm_clear(term_color);
  }

  const uint8_t *glyph = &default_font[(uint8_t)c * 16];

  for (int y = 0; y < 16; ++y)
  {
    for (int x = 0; x < 8; ++x)
    {
      if ((glyph[y] >> (7 - x)) & 1)
      {
        size_t fb_x = cursor_x + x;
        size_t fb_y = cursor_y + y;
        framebuffer[fb_y * (pitch / 4) + fb_x] = fg_color;
      }
    }
  }

  cursor_x += 8;
}

void vterm_print(const char *str)
{
  while (*str)
    vterm_putc(*str++);
}

void vterm_status(const char *str, int ret)
{
  vterm_print(str);
  vterm_print("...");
  if (ret)
  {
    const char *error = __get_err_code(ret);
    kerror(error);
  }
  else
  {
    kok();
  }
}
