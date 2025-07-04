#include "./x86_64/vga_text.h"

void kmain(void)
  {
    set_cursor_pos(0, 0);
    clear_win(COLOR_BLUE, COLOR_BLACK);

    for (;;)
      __asm__ volatile ("cli; hlt");
  }
