#include "idt.h"

struct idt_entry idt[IDT_ENTRIES] = {0};

void idt_set_gate(size_t vector, void *handler, uint8_t ist)
{
  uint64_t ptr = (uint64_t)handler;

  idt[vector].offset_low = (uint16_t)ptr;
  idt[vector].selector = KERNEL_CODE64;
  idt[vector].ist = ist;

  if (vector < 0x10) {
    if (((vector < 0xB) && (vector > 0x6)) || vector == 0x2) {
      idt[vector].attributes = 0x8F;
    } else {
      idt[vector].attributes = 0xEF;
    }
  } else {
    idt[vector].attributes = 0x8E;
  }

  idt[vector].isr_mid = (uint16_t)(ptr >> 16);
  idt[vector].isr_high = (uint32_t)(ptr >> 32);
  idt[vector].reserved = 0;

}

void idt_reload(void)
{
  struct idtr idtr_ptr = {sizeof(idt) - 1, (uint64_t)idt};

  __asm__ volatile("lidtq %0" : : "m"(idtr_ptr));
}
