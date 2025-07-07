#pragma once

#include <stdint.h>

#define KERNEL_CS 0x08
#define INT_ATTR 0x8E
#define IDT_ENTRIES 256

typedef struct __attribute__((packed)) {
  uint16_t limit;
  uint64_t base;
} idt_register;

typedef struct __attribute__((packed)) {
  uint16_t base_low;               // Low 16 bits of jmp address
  uint16_t cs_selector;            // Code segment selector
  uint8_t zero;                    // Always zero
  uint8_t attributes;              // Flag bytes

                                   // Bit 7 - interrupt present
                                   // Bit 6-5 - privelage level
                                   // Bit 4 - set 0 for interrupt gate
                                   // bit 3-0 - 1110 for 32 bit interrupt gate

  uint16_t base_mid;               // Middle 16 bits of jmp address
  uint32_t base_high;              // High 16 bits of jmp address
  uint32_t reserved;               // Always zero
} idt_gate;

extern idt_gate main_idt[IDT_ENTRIES];
extern idt_register main_idt_reg;

void set_idt_gate(uint8_t gate_num, uintptr_t handler_address);
void set_idt();
