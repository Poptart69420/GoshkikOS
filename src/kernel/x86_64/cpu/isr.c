#include "isr.h"
#include "idt.h"
#include "../vga_text.h"
#include <stdint.h>
#include <stddef.h>

char *exception_messages[] = {
  "Division by Zero",
  "Debug",
  "Non-Maskable Interrupt",
  "Breakpoint",
  "Overflow",
  "Out of Bounds",
  "Invalid Opcode",
  "No Coprocessor",

  "Double Fault",
  "Coprocessor Segment Overrun",
  "Bad TSS",
  "Segment not Present",
  "Stack Fault",
  "General Protection Fault",
  "Page Fault",
  "Unknown Interrupt",

  "Coprocessor Fault",
  "Alignment Check",
  "Machine Check",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",

  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved"
};

void isr_install(void)
  {
    set_idt_gate(0, (uintptr_t) isr_0);
    set_idt_gate(1, (uintptr_t) isr_1);
    set_idt_gate(2, (uintptr_t) isr_2);
    set_idt_gate(3, (uintptr_t) isr_3);
    set_idt_gate(4, (uintptr_t) isr_4);
    set_idt_gate(5, (uintptr_t) isr_5);
    set_idt_gate(6, (uintptr_t) isr_6);
    set_idt_gate(7, (uintptr_t) isr_7);
    set_idt_gate(8, (uintptr_t) isr_8);
    set_idt_gate(9, (uintptr_t) isr_9);
    set_idt_gate(10, (uintptr_t) isr_10);
    set_idt_gate(11, (uintptr_t) isr_11);
    set_idt_gate(12, (uintptr_t) isr_12);
    set_idt_gate(13, (uintptr_t) isr_13);
    set_idt_gate(14, (uintptr_t) isr_14);
    set_idt_gate(15, (uintptr_t) isr_15);
    set_idt_gate(16, (uintptr_t) isr_16);
    set_idt_gate(17, (uintptr_t) isr_17);
    set_idt_gate(18, (uintptr_t) isr_18);
    set_idt_gate(19, (uintptr_t) isr_19);
    set_idt_gate(20, (uintptr_t) isr_20);
    set_idt_gate(21, (uintptr_t) isr_21);
    set_idt_gate(22, (uintptr_t) isr_22);
    set_idt_gate(23, (uintptr_t) isr_23);
    set_idt_gate(24, (uintptr_t) isr_24);
    set_idt_gate(25, (uintptr_t) isr_25);
    set_idt_gate(26, (uintptr_t) isr_26);
    set_idt_gate(27, (uintptr_t) isr_27);
    set_idt_gate(28, (uintptr_t) isr_28);
    set_idt_gate(29, (uintptr_t) isr_29);
    set_idt_gate(30, (uintptr_t) isr_30);
    set_idt_gate(31, (uintptr_t) isr_31);

    set_idt();

    __asm__ volatile ("sti");
  }

void isr_handler(uint64_t isr_number, uint64_t error_code, registers* regs)
{
  if (isr_number < 32) {
    putstr(exception_messages[isr_number], COLOR_WHITE, COLOR_RED);
  } else {
    putstr("Unknown Exception", COLOR_WHITE, COLOR_RED);
  }

  __asm__ volatile ("cli; hlt");

}
