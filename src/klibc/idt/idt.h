#ifndef IDT_H_
#define IDT_H_

#include <stddef.h>
#include <stdint.h>

#include "../include/selectors.h"

#include "../include/selectors.h"

#define IDT_ENTRIES 256

struct idt_entry
{
  uint16_t offset_low;
  uint16_t selector;
  uint8_t ist;
  uint8_t attributes;
  uint16_t isr_mid;
  uint32_t isr_high;
  uint32_t reserved;
} __attribute__((packed));

struct idtr
{
  uint16_t limit;
  uint64_t base;
} __attribute__((packed));

#endif // IDT_H_
