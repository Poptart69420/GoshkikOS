#include <arch/x86_64/cpu/gdt/gdt.h>

struct gdtr gdt = {0};
struct gdt_ptr gdt_ptr = {0};
spinlock_t lock;

extern void gdt_reload(void);
extern void tss_reload(void);

void gdt_setup(void)
{
  spinlock_init(&lock);
  gdt_init();
}

void gdt_init(void)
{
  spinlock_acquire(&lock);

  // kernel

  gdt.entries[1].access = 0b10011010;
  gdt.entries[1].granularity = 0b00100000;

  gdt.entries[2].access = 0b10010010;

  // user

  gdt.entries[3].access = 0b11110010;

  gdt.entries[4].access = 0b11111010;
  gdt.entries[4].granularity = 0b001000000;

  // tss

  gdt.tss.length = sizeof(struct tss);
  gdt.tss.flags = 0b10001001;

  // pointer

  gdt_ptr.limit = sizeof(gdt) - 1;
  gdt_ptr.ptr = (uint64_t)&gdt;

  gdt_reload();
  tss_reload();
  spinlock_release(&lock);
  vterm_print("GDT...");
  kok();
}
