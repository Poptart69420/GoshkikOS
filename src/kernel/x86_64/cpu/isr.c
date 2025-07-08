#include "isr.h"
#include "idt.h"
#include "../vga_text.h"
#include "../io.h"
#include <stdint.h>
#include <stddef.h>

static const char *exception_messages[] = {
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

static const char* irq_names[] =
  {
    "Timer",
    "Keyboard",
    "Cascade",
    "COM2",
    "COM1",
    "LPT2",
    "Floppy",
    "LPT1",
    "RTC",
    "ACPI",
    "Unused",
    "Unused",
    "Mouse",
    "FPU",
    "Primary ATA",
    "Secondary ATA"
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
    set_idt_gate(31, (uintptr_t) isr_31);

    set_idt_gate(32, (uintptr_t) irq_0);
    set_idt_gate(33, (uintptr_t) irq_1);
    set_idt_gate(34, (uintptr_t) irq_2);
    set_idt_gate(35, (uintptr_t) irq_3);
    set_idt_gate(36, (uintptr_t) irq_4);
    set_idt_gate(37, (uintptr_t) irq_5);
    set_idt_gate(38, (uintptr_t) irq_6);
    set_idt_gate(39, (uintptr_t) irq_7);
    set_idt_gate(40, (uintptr_t) irq_8);
    set_idt_gate(41, (uintptr_t) irq_9);
    set_idt_gate(42, (uintptr_t) irq_10);
    set_idt_gate(43, (uintptr_t) irq_11);
    set_idt_gate(44, (uintptr_t) irq_12);
    set_idt_gate(45, (uintptr_t) irq_13);
    set_idt_gate(46, (uintptr_t) irq_14);
    set_idt_gate(47, (uintptr_t) irq_15);

    set_idt();

    __asm__ volatile ("sti");
  }

void pit_init(uint32_t hz)  {
    uint16_t divisor = PIT_FREQUENCY / hz;
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
  }

static void ps2_flush_output(uint16_t timeout)
  {
    while (inb(PS2_COMMAND) & 1 && timeout > 0) {
      timeout--;
      inb(PS2_DATA);
    }
  }

static int ps2_wait_input(void)
  {
    uint64_t timeout = 1000;
    while (--timeout) {
      if (!(inb(PS2_COMMAND) & (1 << 1))) return 0;
    }
    return 1;
  }

static int ps2_wait_output(void)
  {
    uint64_t timeout = 1000;
    while (--timeout) {
      if (inb(PS2_COMMAND) & (1 << 0)) return 0;
    }
    return 1;
  }

static void set_kboard_scancode(void)
{
  ps2_wait_input();
  outb(PS2_DATA, 0xF0);
  ps2_wait_output();
  if (inb(PS2_DATA) != 0xFA) {
    putstr("Failed to send 0xF0\n", COLOR_RED, COLOR_BLACK);
    return;
  }

  ps2_wait_input();
  outb(PS2_DATA, 0x02);
  ps2_wait_output();
  if (inb(PS2_DATA) != 0xFA) {
    putstr("Failed to send scancode set\n", COLOR_RED, COLOR_BLACK);
  }
}

void ps2_setup(void)
  {
    outb(PS2_COMMAND, PS2_DISABLE_P1);
    outb(PS2_COMMAND, PS2_DISABLE_P2);

    ps2_flush_output(PACKETS_IN_PIPE);

    uint8_t status;

    ps2_wait_input();

    status = inb(PS2_DATA);

    status &= ~0x30;
    status |= 0x01;

    ps2_wait_input();
    outb(PS2_COMMAND, PS2_DATA);
    ps2_wait_input();
    outb(PS2_DATA, status);

    outb(PS2_COMMAND, PS2_ENABLE_P1);
    outb(PS2_COMMAND, PS2_ENABLE_P2);
  }

static void ps2_keyboard_handler() {
  uint8_t scancode = inb(PS2_DATA);
  putstr("Keyboard", COLOR_YELLOW, COLOR_BLACK);

}

void irq_handler(uint64_t vector)
  {
    switch (vector) {

      case 0:
        break;
      case 1:
        ps2_keyboard_handler();
        break;
      default:
        putstr(irq_names[vector], COLOR_YELLOW, COLOR_BLACK);
        break;
    }

    if (vector >= 8) {
      outb(PIC_2, END_OF_INT);
    }

    outb(PIC_1, END_OF_INT);
    return;
  }

void isr_handler(uint64_t vector, uint64_t error_code, registers* regs)
{
  if (vector >= IRQ_VECTR_OFFST && vector <= ISR_COUNT) {
    irq_handler(vector - IRQ_VECTR_OFFST);
    return;

  } else if (vector < IRQ_VECTR_OFFST) {
    putstr(exception_messages[vector], COLOR_WHITE, COLOR_RED);
  } else {
    putstr("Unknown Exception", COLOR_WHITE, COLOR_RED);
  }

  return;
}
