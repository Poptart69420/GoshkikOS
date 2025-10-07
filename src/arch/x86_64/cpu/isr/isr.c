#include <arch/x86_64/cpu/isr/isr.h>

static handlers_t handlers[IDT_ENTRIES] = {NULL};

void isr_register(int isr, handlers_t handler)
{
  handlers[isr] = handler;
}

static const char *isr_exception_messages[] = {"Divide by zero",
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
                                               "FPU error"};

extern void *isr_stub_table[];

void kpanic(const char *error, fault_frame_t *frame)
{
  (void)error;
  (void)frame;
  disable_interrupt();
  hcf();
}

void isr_install(void)
{

  for (int i = 0; i < IDT_ENTRIES; ++i)
  {
    uint8_t ist = 0;
    uint8_t attr = IDT_INT_KERNEL;

    if (i == 14)
    {
      ist = 2;
    }

    if (i == 0x80)
    {
      attr = IDT_INT_USER;
    }

    idt_set_gate(i, isr_stub_table[i], ist, attr);
  }

  idt_reload();
  vterm_print("ISR...");
  kok();
}

void isr_handler(fault_frame_t *frame)
{
  if (frame->cs & 0x3)
  {
    __asm__ volatile("swapgs" ::: "memory");
  }

  if (frame->err_type < IDT_ENTRIES && handlers[frame->err_type] != NULL)
  {
    handlers[frame->err_type](frame);
  }

  if (frame->err_type < 32)
  {
    if (frame->cs & 0x3)
    {
      vterm_print("\n");
      vterm_print("-----EXCEPTION-----\n");
      kerror(isr_exception_messages[frame->err_type]);
      // do something
    }
    else
    {
      vterm_print("\n");
      vterm_print("-----KERNEL EXCEPTION-----\n");

      kpanic(isr_exception_messages[frame->err_type], frame);
    }
  }

  if (frame->cs & 0x3)
  {
    __asm__ volatile("swapgs" ::: "memory");
  }
}
