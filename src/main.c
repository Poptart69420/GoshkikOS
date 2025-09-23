#include "kernel.h"

kernel_table master_kernel_table;
kernel_table *kernel;

static void boot_info(void)
{
  kernel = &master_kernel_table;

  if (!framebuffer_request.response) hcf();
  if (!hhdm_request.response)        hcf();
  if (!memmap_request.response)      hcf();
  if (!module_request.response)      hcf();

  kernel->framebuffer = framebuffer_request.response;
  kernel->hhdm        = hhdm_request.response;
  kernel->memmap      = memmap_request.response;
  kernel->module      = module_request.response;

  if (LIMINE_BASE_REVISION_SUPPORTED == false) {
    hcf();
  }

  if (kernel->framebuffer == NULL
      || kernel->framebuffer->framebuffer_count < 1) {
    hcf();
  }

  if (!kernel->module || kernel->module->module_count == 0) {
    hcf();
  }
}

void test_initrd_read(void)
{
  const char *path = "/initrd/etc/test.txt";
  vfs_node_t *file = vfs_lookup(path);

  if (!file) {
    vterm_print("File not found\n");
    return;
  }

  if (vfs_open(file) != 0) {
    vterm_print("VFS: Failed to open file\n");
  }

  if (file->size == 0) {
    vterm_print("VFS: File empty\n");
    vfs_close(file);
    return;
  }

  char *buf = kmalloc(file->size + 1);
  if (!buf) hcf();

  size_t read_bytes = vfs_read(file, 0, file->size, buf);
  if (read_bytes == (size_t)-1 || read_bytes > file->size) {
    vterm_print("VFS: Failed to read\n");
    kfree(buf);
    vfs_close(file);
    return;
  }

  buf[read_bytes] = '\0';
  vterm_print("\nContents of file \"test.txt\":\n");
  vterm_print(buf);

  kfree(buf);
  vfs_close(file);
}

void test_initrd_read2(void)
{
  const char *path = "/initrd/etc/dwadawd.txt";
  vfs_node_t *file = vfs_lookup(path);

  if (!file) {
    vterm_print("File not found\n");
    return;
  }

  if (vfs_open(file) != 0) {
    vterm_print("VFS: Failed to open file\n");
  }

  if (file->size == 0) {
    vterm_print("VFS: File empty\n");
    vfs_close(file);
    return;
  }

  char *buf = kmalloc(file->size + 1);
  if (!buf) hcf();

  size_t read_bytes = vfs_read(file, 0, file->size, buf);
  if (read_bytes == (size_t)-1 || read_bytes > file->size) {
    vterm_print("VFS: Failed to read\n");
    kfree(buf);
    vfs_close(file);
    return;
  }

  buf[read_bytes] = '\0';
  vterm_print("\nContents of file \"dwadawd.txt\":\n");
  vterm_print(buf);

  kfree(buf);
  vfs_close(file);
}

void kmain(void)
{
  boot_info();

  struct limine_framebuffer *fb = kernel->framebuffer->framebuffers[0];
  vterm_init(fb);

  vterm_print("\n");

  vterm_print("=|||||||||||||||||||=\n");
  vterm_print("|                   |\n");
  vterm_print("| GoshkikOS Started |\n");
  vterm_print("|                   |\n");
  vterm_print("=|||||||||||||||||||=\n");
  vterm_print("\n");

  g_hhdm_offset = hhdm_request.response->offset;
  
  init_memmap(memmap_request.response);
  init_pmm();
  init_vmm();
  init_kheap();

  gdt_init();
  isr_install();
  pic_remap(0x20, 0x28);
  pit_init();
  ps2_entry();
  init_keyboard();
  init_timer();
  init_vfs();
  vfs_register_filesystem(&fs_ramfs);
  vfs_mount("ramfs", NULL, "/");

  mount_initrd();

  vfs_ls("/initrd");

  test_initrd_read();
  test_initrd_read2();

  pic_unmask_irq(0);
  pic_unmask_irq(1);

  __asm__ volatile ("sti");

  hcf();
}
