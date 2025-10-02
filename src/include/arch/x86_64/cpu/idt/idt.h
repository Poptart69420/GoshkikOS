#ifndef IDT_H_
#define IDT_H_

#include <stddef.h>
#include <stdint.h>

#include <arch/x86_64/selectors/selectors.h>

#define IDT_ENTRIES 256

#define IDT_TYPE_INTERRUPT 0xE
#define IDT_TYPE_TRAP 0xF

#define IDT_PRESENT 0x80
#define IDT_DPL0 0x00
#define IDT_DPL3 0x60

// Common
#define IDT_INT_KERNEL (IDT_PRESENT | IDT_DPL0 | IDT_TYPE_INTERRUPT) // 0x8E
#define IDT_INT_USER (IDT_PRESENT | IDT_DPL3 | IDT_TYPE_INTERRUPT)   // 0xEE
#define IDT_TRAP_KERNEL (IDT_PRESENT | IDT_DPL0 | IDT_TYPE_TRAP)     // 0x8F
#define IDT_TRAP_USER (IDT_PRESENT | IDT_DPL3 | IDT_TYPE_TRAP)       // 0xEF

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
} __attribute__((packed));

struct idtr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

void idt_set_gate(size_t vector, void *handler, uint8_t ist, uint8_t type_attr);
void idt_reload(void);

#endif // IDT_H_
