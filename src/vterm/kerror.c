#include <vterm/kerror.h>

void kerror(const char *msg)
{
  vterm_set_fg_color(COLOR_RED);
  kprintf("Error: %s\n", msg);
  vterm_set_fg_color(COLOR_WHITE);
}
