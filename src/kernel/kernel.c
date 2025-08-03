#include "./x86_64/vga_text.h"
#include "./x86_64/cpu/isr.h"
#include "./x86_64/cpu/pic.h"
#include "./x86_64/cpu/kbd.h"
#include <stdint.h>

#define TIMER_HZ 100

void kmain(void)
  {
    set_cursor_pos(0, 0);
    clear_win(COLOR_GREEN, COLOR_BLACK);
    const char *msg_kernel_enter = "ENTERED SHITOS KERNEL...\n";
    putstr(msg_kernel_enter, COLOR_GREEN, COLOR_BLACK);

    pic_mask_irq(0xFF);
    pic_remap(PIC1_COMMAND, 0x28);
    pit_init(TIMER_HZ);
    ps2_setup();

    isr_install();

    for (;;)
      __asm__ volatile ("hlt");
  }
