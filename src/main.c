#include "include/klibc/printf.h"
#include <kernel.h>

kernel_table master_kernel_table;
kernel_table *kernel;

static void boot_info(void)
{
  kernel = &master_kernel_table;

  if (!framebuffer_request.response) // No framebuffer response
    hcf();                           // Sad
  if (!hhdm_request.response)        // No HHDM response
    hcf();                           // Sad
  if (!memmap_request.response)      // No memmap response
    hcf();                           // Sad
  if (!module_request.response)      // No module response (initrd.tar)
    hcf();                           // Sas

  // Set kernel table responses
  kernel->framebuffer = framebuffer_request.response;
  kernel->hhdm = hhdm_request.response;
  kernel->memmap = memmap_request.response;
  kernel->module = module_request.response;

  if (LIMINE_BASE_REVISION_SUPPORTED == false) // Revision is not supported
  {
    hcf(); // Sad
  }

  if (kernel->framebuffer == NULL || kernel->framebuffer->framebuffer_count < 1) // If framebuffer is shit
  {
    hcf(); // Sad
  }

  if (!kernel->module || kernel->module->module_count == 0) // If no module in table, or no module provided (no initrd.tar)
  {
    hcf(); // Sad
  }
}

//
// TODO: Move this stuff to a test directory. I think having a single header that contains every other test header
// so that it is easy to include in this file, and remove as needed. (only include in testing builds).
// if having a single "#ifdef DEBUG #include "../tests/all_tests.h" #endif" doesn't add tests to the binary
// that would also work
//
static void task_1(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  for (;;)
  {
    kprintf("Thread_1: Hello\n");
  }
}

static void task_2(int argc, char **argv)
{
  (void)argc;
  (void)argv;

  for (;;)
  {
    kprintf("Thread_2: Hiii\n");
  }
}

static void test_scheduler(void)
{
  thread_t *thread_1 = thread_create(task_1, 0, NULL, THREAD_RING_0, THREAD_PRIO_LOW, 0);
  if (!thread_1)
  {
    kerror("Failed to create thread_1");
    hcf();
  }

  thread_t *thread_2 = thread_create(task_2, 0, NULL, THREAD_RING_0, THREAD_PRIO_LOW, 0);
  if (!thread_2)
  {
    kerror("Failed to create thread_2");
    hcf();
  }
}

//
// Main function, called by bootloader (Limine)
//
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
  kprintf("| GoshkikOS Started |\n");
  kprintf("|                   |\n");
  kprintf("=|||||||||||||||||||=\n");
  kprintf("\n");

  g_hhdm_offset = hhdm_request.response->offset; // Why is this here?

  init_mmu(memmap_request.response);
  init_pmm();
  init_vmm();
  init_kheap();
  // init_vfs(); //TODO: Implement a VFS

  init_serial();
  init_gdt();
  init_idt();
  pic_remap(0x20, 0x28);
  init_pit();
  init_system_clock();
  init_ps2();
  init_syscalls();
  init_threading();
  init_kprintf_spinlock();

  enable_interrupt();
  //test_scheduler(); // Test thing
  halt();           // Halt with inturrupts enabled
}
