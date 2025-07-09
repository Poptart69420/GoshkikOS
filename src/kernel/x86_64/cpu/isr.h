#pragma once

#include <stdint.h>

extern void isr_0();
extern void isr_1();
extern void isr_2();
extern void isr_3();
extern void isr_4();
extern void isr_5();
extern void isr_6();
extern void isr_7();
extern void isr_8();
extern void isr_9();
extern void isr_10();
extern void isr_11();
extern void isr_12();
extern void isr_13();
extern void isr_14();
extern void isr_15();
extern void isr_16();
extern void isr_17();
extern void isr_18();
extern void isr_19();
extern void isr_20();
extern void isr_21();
extern void isr_22();
extern void isr_23();
extern void isr_24();
extern void isr_25();
extern void isr_26();
extern void isr_27();
extern void isr_28();
extern void isr_29();
extern void isr_30();
extern void isr_31();
extern void irq_0();
extern void irq_1();
extern void irq_2();
extern void irq_3();
extern void irq_4();
extern void irq_5();
extern void irq_6();
extern void irq_7();
extern void irq_8();
extern void irq_9();
extern void irq_10();
extern void irq_11();
extern void irq_12();
extern void irq_13();
extern void irq_14();
extern void irq_15();

typedef struct __attribute__((packed)) {
  uint64_t r15, r14, r13, r12, rbp, rbx;
  uint64_t r11, r10, r9, r8, rax, rcx, rdx, rsi, rdi;
  uint64_t irq_number, error_code;
  uint64_t rip, cs, eflags, rsp, ss;
} registers;

#define PIT_FREQUENCY   1193182
#define PIT_CHANNEL0    0x40
#define PIT_COMMAND     0x43

#define PIC_1           0x20
#define PIC_2           0xA0

#define END_OF_INT      0x20
#define IRQ_VECTR_OFFST 32
#define ISR_COUNT       47

void isr_handler(uint64_t isr_number, uint64_t error_code, registers* regs);
void isr_install(void);
void pit_init(uint32_t hz);
