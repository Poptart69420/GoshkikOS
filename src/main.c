#include <kernel.h>

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

typedef struct my_object_t {
    int id;
    char buffer[56];
} my_object_t;

void kmain(void) {
    boot_info();

    struct limine_framebuffer *fb = kernel->framebuffer->framebuffers[0];
    if (fb->width > FB_WIDTH || fb->height > FB_HEIGHT)
        hcf();
    vterm_init(fb);

    vterm_print("\n");

    vterm_print("=|||||||||||||||||||=\n");
    vterm_print("|                   |\n");
    vterm_print("| GoshkikOS Started |\n");
    vterm_print("|                   |\n");
    vterm_print("=|||||||||||||||||||=\n");
    vterm_print("\n");

    g_hhdm_offset = hhdm_request.response->offset;

    init_mmu(memmap_request.response);
    init_pmm();
    init_vmm();
    init_kheap();

    init_serial();
    gdt_setup();
    isr_install();
    pic_remap(0x20, 0x28);
    pit_init();
    ps2_entry();
    init_keyboard();
    init_timer();

    pic_unmask_irq(0);
    pic_unmask_irq(1);

    enable_interrupt();
    halt();
}
