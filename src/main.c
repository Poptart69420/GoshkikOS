#include "kernel.h"

kernel_table master_kernel_table;
kernel_table *kernel;

static void boot_info(void) {
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

    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    if (kernel->framebuffer == NULL ||
        kernel->framebuffer->framebuffer_count < 1) {
        hcf();
    }

    if (!kernel->module || kernel->module->module_count == 0) {
        hcf();
    }
}

static void ls(const char *path) {
    vfs_node_t *node = vfs_open(path, VFS_READONLY);
    if (!node) {
        vterm_print("Path ");
        vterm_print(path);
        kerror("doesn't exist");
        return;
    }

    vterm_print("ls ");
    vterm_print(path);
    vterm_print("\n");

    uint64_t index = 0;
    struct dirent_t *ret;
    while ((ret = vfs_readdir(node, index)) != NULL) {
        vterm_print(ret->d_name);
        vterm_print("\n");
        kfree(ret);
        index++;
    }

    vfs_close(node);
}

static void cat(const char *path) {
    vfs_node_t *node = vfs_open(path, VFS_READONLY);
    if (!node) {
        vterm_print("File ");
        vterm_print(path);
        kerror(" not found");
        return;
    }

    vterm_print("cat ");
    vterm_print(path);
    vterm_print("\n");

    if (!(node->flags & VFS_FILE)) {
        kerror("Not a file");
        vfs_close(node);
        return;
    }

    char *buffer = kmalloc(node->size + 1);
    if (!buffer) {
        kerror("Out of memory");
        vfs_close(node);
        return;
    }

    uint64_t bytes = vfs_read(node, buffer, 0, node->size);

    if (bytes == 0) {
        vterm_print("no data read\n");
        kfree(buffer);
        vfs_close(node);
        return;
    }

    buffer[bytes] = '\0';
    vterm_print(buffer);
    vterm_print("\n");

    kfree(buffer);
    vfs_close(node);
}

void kmain(void) {
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

    init_tmpfs();

    mount_initrd();

    vterm_print("\n");

    ls("/initrd/etc");

    vterm_print("\n");

    cat("/initrd/etc/dwadawd.txt");

    pic_unmask_irq(0);
    pic_unmask_irq(1);

    __asm__ volatile("sti");

    hcf();
}
