#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>

#include "klibc/mem/mem.h"
#include "klibc/gdt/gdt.h"
#include "klibc/isr/isr.h"
#include "klibc/pic/pic.h"
#include "klibc/drivers/vterm/vterm.h"
#include "klibc/pit/pit.h"
#include "klibc/drivers/ps2/ps2.h"
#include "klibc/drivers/ps2/kbd.h"
#include "klibc/timer/timer.h"
#include "klibc/include/global.h"
#include "klibc/mem/memmap.h"
#include "klibc/mem/pmm.h"
#include "klibc/limine_requests/limine_requests.h"
#include "klibc/include/hcf.h"

void kmain(void)
{
  if (LIMINE_BASE_REVISION_SUPPORTED == false) {
    hcf();
  }

  if (framebuffer_request.response == NULL
      || framebuffer_request.response->framebuffer_count < 1) {
    hcf();
  }
  struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

  vterm_init(framebuffer);

  vterm_print("\n");

  vterm_print("|----------------|\n");
  vterm_print("| ShitOS Started |\n");
  vterm_print("|----------------|\n");

  g_hhdm_offset = hhdm_request.response->offset;
  
  init_memmap(memmap_request.response);
  init_pmm();

  gdt_init();
  isr_install();
  pic_remap(0x20, 0x28);
  pit_init();
  ps2_entry();
  init_keyboard();
  init_timer();

  pic_unmask_irq(0);
  pic_unmask_irq(1);

  __asm__ volatile ("sti");

  hcf();
}
