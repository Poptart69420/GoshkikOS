#include <arch/x86_64/cpu/idt/idt.h>

struct idtr idt_ptr;
idt_entry_t idt_entries[256];

// Assembly stubs
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);

extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

static void *irq_stubs[] = {
    irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7,
    irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15};

void idt_set_entry(int index, uint64_t handler, uint16_t selector, uint8_t flags)
{
  idt_entries[index].offset_low = handler & 0xFFFF;
  idt_entries[index].selector = selector;
  idt_entries[index].ist = 0;
  idt_entries[index].type_attr = flags;
  idt_entries[index].offset_mid = (handler >> 16) & 0xFFFF;
  idt_entries[index].offset_high = (handler >> 32) & 0xFFFFFFFF;
  idt_entries[index].reserved = 0;
}

void init_idt(void)
{
  kprintf("IDT...");
  idt_ptr.limit = sizeof(idt_entries) - 1;
  idt_ptr.base = (uint64_t)&idt_entries;

  for (int i = 0; i < 256; i++)
    idt_set_entry(i, 0, 0, 0);

  // Exceptions
  idt_set_entry(0, (uint64_t)isr0, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(1, (uint64_t)isr1, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(2, (uint64_t)isr2, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(3, (uint64_t)isr3, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(4, (uint64_t)isr4, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(5, (uint64_t)isr5, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(6, (uint64_t)isr6, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(7, (uint64_t)isr7, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(8, (uint64_t)isr8, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(9, (uint64_t)isr9, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(10, (uint64_t)isr10, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(11, (uint64_t)isr11, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(12, (uint64_t)isr12, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(13, (uint64_t)isr13, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(14, (uint64_t)isr14, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(15, (uint64_t)isr15, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(16, (uint64_t)isr16, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(17, (uint64_t)isr17, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(18, (uint64_t)isr18, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);
  idt_set_entry(19, (uint64_t)isr19, KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);

  // IRQs
  for (int i = 0; i < 16; i++)
    idt_set_entry(32 + i, (uint64_t)irq_stubs[i], KERNEL_CODE64, IDT_TYPE_INTERRUPT_GATE);

  __asm__ volatile("lidt %0" : : "m"(idt_ptr));
  kok();
}
