#include <vterm/kok.h>

void kok(void)
{
  vterm_set_fg_color(COLOR_GREEN);
  vterm_print("OK\n");
  vterm_set_fg_color(COLOR_WHITE);
}
