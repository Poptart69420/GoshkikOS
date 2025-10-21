#include <arch/x86_64/cpu/gdt/gdt.h>

struct gdtr gdt = {0};
struct gdt_ptr gdt_ptr = {0};
spinlock_t lock;

extern void gdt_reload(void);
extern void tss_reload(void);

tss_t tss = {0};

void gdt_setup(void)
{
  kprintf("GDT...");
  spinlock_acquire(&lock);

  // Kernel segments
  gdt.entries[1].access = 0b10011010;
  gdt.entries[1].granularity = 0b00100000;

  gdt.entries[2].access = 0b10010010;
  gdt.entries[2].granularity = 0;

  // User segments
  gdt.entries[3].access = 0b11110010;
  gdt.entries[3].granularity = 0;

  gdt.entries[4].access = 0b11111010;
  gdt.entries[4].granularity = 0b00100000;

  // TSS
  gdt.tss.length = sizeof(tss_t);
  gdt.tss.flags = 0b10001001;

  uintptr_t tss_base = (uintptr_t)&tss;
  gdt.tss.base_low = tss_base & 0xFFFF;
  gdt.tss.base_mid = (tss_base >> 16) & 0xFF;
  gdt.tss.base_high = (tss_base >> 24) & 0xFF;
  gdt.tss.base_upper32 = (tss_base >> 32) & 0xFFFFFFFF;

  gdt_ptr.limit = sizeof(gdt) - 1;
  gdt_ptr.ptr = (uint64_t)&gdt;

  gdt_reload();
  tss_reload();
  spinlock_release(&lock);
  kok();
}

void init_gdt(void)
{
  spinlock_init(&lock);
  gdt_setup();
}
