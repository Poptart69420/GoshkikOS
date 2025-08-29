#include "idt.h"

struct idt_entry idt[IDT_ENTRIES] = {0};

void idt_set_gate(size_t vector, void *handler, uint8_t ist, uint8_t type_attr)
{
  uint64_t ptr = (uint64_t)handler;

  idt[vector].offset_low  = (uint16_t)(ptr & 0xFFFF);
  idt[vector].selector    = KERNEL_CODE64;
  idt[vector].ist         = ist & 0x7;
  idt[vector].attributes  = type_attr;
  idt[vector].isr_mid     = (uint16_t)((ptr >> 16) & 0xFFFF);
  idt[vector].isr_high    = (uint32_t)((ptr >> 32) & 0xFFFFFFFF);
  idt[vector].reserved    = 0;
}

void idt_reload(void)
{
  struct idtr idtr_ptr = {sizeof(idt) - 1, (uint64_t)idt};

  __asm__ volatile("lidtq %0" : : "m"(idtr_ptr));
}
