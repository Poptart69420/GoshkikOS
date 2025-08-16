#include "idt.h"
#include <stdint.h>
#include <stdlib.h>

idt_gate main_idt[IDT_ENTRIES];
idt_register main_idt_reg;

void set_idt()
  {
    main_idt_reg.base = (uintptr_t) &main_idt;
    main_idt_reg.limit = (IDT_ENTRIES * sizeof(idt_gate)) - 1;

    __asm__ volatile ("lidt (%0)"
                      :
                      :
                      "r" (&main_idt_reg));
  }

void set_idt_gate(uint8_t gate_num, uintptr_t handler_address)
  {
    uint16_t low = (uint16_t) (handler_address & 0xFFFF);
    uint16_t middle = (uint16_t) ((handler_address >> 16) & 0xFFFF);
    uint32_t high = (uint32_t) (handler_address >> 32 & 0xFFFFFFFF);

    idt_gate gate = {
      .base_low = low,
      .cs_selector = KERNEL_CS,
      .zero = 0,
      .attributes = INT_ATTR,
      .base_mid = middle,
      .base_high = high,
      .reserved = 0
    };

    main_idt[gate_num] = gate;
  }
