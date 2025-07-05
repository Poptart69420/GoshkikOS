#include "./x86_64/vga_text.h"

void kmain(void)
  {
    set_cursor_pos(0, 0);
    clear_win(COLOR_GREEN, COLOR_BLACK);
    const char *msg_kernel_enter = "ENTERED SHITOS KERNEL...";
    putstr(msg_kernel_enter, COLOR_GREEN, COLOR_BLACK);

    for (;;)
      __asm__ volatile ("cli; hlt");
  }
