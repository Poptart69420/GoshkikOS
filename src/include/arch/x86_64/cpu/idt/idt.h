#ifndef IDT_H_
#define IDT_H_

#include <stddef.h>
#include <stdint.h>

#include <arch/x86_64/selectors/selectors.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

#define IDT_ENTRIES 256

// Common
#define IDT_TYPE_INTERRUPT_GATE 0x8E // 32-bit interrupt gate, ring 0
#define IDT_TYPE_TRAP_GATE 0x8F      // 32-bit trap gate, ring 0
#define IDT_TYPE_TASK_GATE 0x85      // Task gate
#define IDT_TYPE_USER_INT_GATE 0xEE  // 32-bit interrupt gate, ring 3

// IDT Entry Structure (64-bit)
typedef struct
{
  uint16_t offset_low;  // Lower 16 bits of handler address
  uint16_t selector;    // Kernel segment selector
  uint8_t ist;          // Interrupt Stack Table offset
  uint8_t type_attr;    // Type and attributes
  uint16_t offset_mid;  // Middle 16 bits of handler address
  uint32_t offset_high; // Upper 32 bits of handler address
  uint32_t reserved;    // Reserved, must be zero
} __attribute__((packed)) idt_entry_t;

struct idtr
{
  uint16_t limit;
  uint64_t base;
} __attribute__((packed));

void init_idt(void);
void idt_set_entry(int index, uint64_t handler, uint16_t selector, uint8_t flags);

#endif // IDT_H_
