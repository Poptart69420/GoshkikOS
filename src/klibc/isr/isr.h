#ifndef ISR_H_
#define ISR_H_

const char *isr_exception_messages[] =
{
  "Divide by zero",
  "Debug",
  "NMI",
  "Breakpoint",
  "Overflow",
  "Bound Range Exceeded",
  "Invalid Opcode",
  "Device Not Available",
  "Double fault",
  "Co-processor Segment Overrun",
  "Invalid TSS",
  "Segment not present",
  "Stack-Segment Fault",
  "GPF",
  "Page Fault",
  "Reserved",
  "x87 Floating Point Exception",
  "alignment check",
  "Machine check",
  "SIMD floating-point exception",
  "Virtualization Exception",
  "Deadlock",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Security Exception",
  "Reserved",
  "Triple Fault",
  "FPU error"
};

#endif // ISR_H_
