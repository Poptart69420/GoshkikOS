#include <vterm/kok.h>

void kok(void)
{
  vterm_set_fg_color(COLOR_GREEN);
  kprintf("OK\n");
  vterm_set_fg_color(COLOR_WHITE);
}
