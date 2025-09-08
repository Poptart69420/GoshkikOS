#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>

#include "klibc/mem.h"
#include "klibc/gdt/gdt.h"
#include "klibc/isr/isr.h"
#include "klibc/pic/pic.h"
#include "klibc/drivers/vterm/vterm.h"
#include "klibc/pit/pit.h"
#include "klibc/drivers/ps2/ps2.h"
#include "klibc/drivers/ps2/kbd.h"
#include "klibc/timer/timer.h"
#include "klibc/paging/paging.h"

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request =
{
  .id = LIMINE_FRAMEBUFFER_REQUEST,
  .revision = 0
};

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

static void hcf(void)
{
  for (;;) {
    __asm__ volatile ("hlt");
  }
}

__attribute__((used, section(".limine_requests")))
static volatile struct limine_executable_address_request kaddr_req =
{
  .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST,
  .revision = 0
};

extern char _kernel_start, _kernel_end;

void kmain(void)
{
  if (LIMINE_BASE_REVISION_SUPPORTED == false) {
    hcf();
  }

  if (framebuffer_request.response == NULL
      || framebuffer_request.response->framebuffer_count < 1) {
    hcf();
  }

  if (kaddr_req.response == NULL) {
    hcf();
  }

  uint64_t k_physical_base = kaddr_req.response->physical_base;
  uint64_t k_virtual_base  = kaddr_req.response->virtual_base;

  uint64_t k_physical_size = (uintptr_t)&_kernel_end - (uintptr_t)&_kernel_start;

  paging_map_kernel(k_physical_base, k_virtual_base, k_physical_size, MAP_SIZE);

  struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

  vterm_init(framebuffer);

  gdt_init();
  isr_install();
  pic_remap(0x20, 0x28);
  pit_init();
  ps2_entry();
  init_keyboard();
  init_timer();

  vterm_print("ShitOS Started\n");

  pic_unmask_irq(0);
  pic_unmask_irq(1);

  __asm__ volatile ("sti");

  hcf();
}
