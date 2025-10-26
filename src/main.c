#include <kernel.h>

kernel_table master_kernel_table;
kernel_table *kernel;

struct timeval time = {
    .tv_sec = 0,
    .tv_usec = 0,
};

static void boot_info(void)
{
  kernel = &master_kernel_table;

  if (!framebuffer_request.response)
    hcf();
  if (!hhdm_request.response)
    hcf();
  if (!memmap_request.response)
    hcf();
  if (!module_request.response)
    hcf();

  kernel->framebuffer = framebuffer_request.response;
  kernel->hhdm = hhdm_request.response;
  kernel->memmap = memmap_request.response;
  kernel->module = module_request.response;

  if (LIMINE_BASE_REVISION_SUPPORTED == false)
  {
    hcf();
  }

  if (kernel->framebuffer == NULL ||
      kernel->framebuffer->framebuffer_count < 1)
  {
    hcf();
  }

  if (!kernel->module || kernel->module->module_count == 0)
  {
    hcf();
  }
}

void kmain(void)
{
  boot_info();

  struct limine_framebuffer *fb = kernel->framebuffer->framebuffers[0];
  if (fb->width > FB_WIDTH || fb->height > FB_HEIGHT)
    hcf();

  vterm_init(fb);

  kprintf("\n");

  kprintf("=|||||||||||||||||||=\n");
  kprintf("|                   |\n");
  kprintf("| goshkikos started |\n");
  kprintf("|                   |\n");
  kprintf("=|||||||||||||||||||=\n");
  kprintf("\n");

  g_hhdm_offset = hhdm_request.response->offset;

  init_mmu(memmap_request.response);
  init_pmm();
  init_vmm();
  init_kheap();
  // init_vfs();

  init_serial();
  init_gdt();
  init_idt();
  pic_remap(0x20, 0x28);
  init_pit();
  init_system_clock();
  init_ps2();
  init_syscalls();
  init_threading();

  pic_unmask_irq(0);

  enable_interrupt();
  halt();
}
