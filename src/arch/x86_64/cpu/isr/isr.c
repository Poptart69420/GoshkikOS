#include <arch/x86_64/cpu/isr/isr.h>

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

void isr_handler(fault_frame_t *frame)
{
  __asm__ volatile("cli");

  kprintf("\n");
  kprintf("EXCEPTION\n");
  kprintf("---------------\n");

  if (!frame)
  {
    kprintf("Null Fault Frame\n");
    hcf();
  }

  kprintf("---Exception Details---\n");
  kprintf("Exception Number: %u (0x%x)\n", frame->int_no, frame->int_no);
  kprintf("Error Code: 0x%lx\n", frame->err_code);

  switch (frame->int_no)
  {
  case 0:
    kprintf("Type: %s\n", isr_exception_messages[0]);
    kprintf("Cause: Division by zero or division overflow\n");
    break;
  case 1:
    kprintf("Type: %s\n", isr_exception_messages[1]);
    kprintf("Cause: Debug symbol\n");
    break;
  case 2:
    kprintf("Type: %s\n", isr_exception_messages[2]);
    kprintf("Cause: Non-maskable interrupt\n");
    break;
  case 3:
    kprintf("Type: %s\n", isr_exception_messages[3]);
    kprintf("Cause: Breakpoint instruction\n");
    break;
  case 4:
    kprintf("Type: %s\n", isr_exception_messages[4]);
    kprintf("Cause: Overflow when executing INTO instruction\n");
    break;
  case 5:
    kprintf("Type: %s\n", isr_exception_messages[5]);
    kprintf("Cause: Index out of bounds\n");
    break;
  case 6:
    kprintf("Type: %s\n", isr_exception_messages[6]);
    kprintf("Cause: Undefined instruction or corrupted code\n");
    break;
  case 7:
    kprintf("Type: %s\n", isr_exception_messages[7]);
    kprintf("Cause: FPU instruction attempted, but no FPU\n");
    break;
  case 8:
    kprintf("Type: %s\n", isr_exception_messages[8]);
    kprintf("Cause: Exception during exception handling\n");
    break;
  case 9:
    kprintf("Type: %s\n", isr_exception_messages[9]);
    kprintf("Cause: Outdated hardware exception\n");
    break;
  case 10:
    kprintf("Type: %s\n", isr_exception_messages[10]);
    kprintf("Cause: Invalid segment selector\n");
    break;
  case 11:
    kprintf("Type: %s\n", isr_exception_messages[11]);
    kprintf("Cause: References a descriptor which is not present, but is marked as present\n");
    break;
  case 12:
    kprintf("Type: %s\n", isr_exception_messages[12]);
    kprintf("Cause: Stack corruption or other stack failure\n");
    break;
  case 13:
    kprintf("Type: %s\n", isr_exception_messages[13]);
    kprintf("Cause: Segment or privilege violation\n");
    break;
  case 14:
    kprintf("Type: %s\n", isr_exception_messages[14]);
    kprintf("Cause: Invalid memory access\n");
    uint64_t cr2;
    __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
    kprintf("Fault Address (CR2): 0x%lx\n", cr2);
    kprintf("Page Fault Flags:\n");
    kprintf("Present: %s\n", (frame->err_code & 1) ? "Yes" : "No");
    kprintf("Write: %s\n", (frame->err_code & 2) ? "Yes" : "No");
    kprintf("User: %s\n", (frame->err_code & 4) ? "Yes" : "No");
    break;
  case 15:
    kprintf("Type: %s\n", isr_exception_messages[15]);
    kprintf("Cause: Reserved\n");
    break;
  case 16:
    kprintf("Type: %s\n", isr_exception_messages[16]);
    kprintf("Cause: Waiting floating point instruction executed and failed\n");
    break;
  case 17:
    kprintf("Type: %s\n", isr_exception_messages[17]);
    kprintf("Cause: Unaligned memory acessed/referenced\n");
    break;
  case 18:
    kprintf("Type: %s\n", isr_exception_messages[18]);
    kprintf("Cause: Processor detected internal errors\n");
    break;
  case 19:
    kprintf("Type: %s\n", isr_exception_messages[19]);
    kprintf("Cause: Unmasked 128-bit media floating point exception and OSXMMEXCPT bit is set to 1\n");
    break;
  case 20:
    kprintf("Type: %s\n", isr_exception_messages[20]);
    kprintf("Cause: ???\n");
    break;
  case 21:
    kprintf("Type: %s\n", isr_exception_messages[21]);
    kprintf("Cause: ???\n");
    break;
  case 22:
  case 23:
  case 24:
  case 25:
  case 26:
  case 27:
    kprintf("Type: %s\n", isr_exception_messages[27]);
    kprintf("Cause: Reserved\n");
    break;
  case 28:
    kprintf("Type: %s\n", isr_exception_messages[28]);
    kprintf("Cause: ???\n");
    break;
  case 29:
    kprintf("Type: %s\n", isr_exception_messages[29]);
    kprintf("Cause: ???\n");
    break;
  case 30:
    kprintf("Type: %s\n", isr_exception_messages[30]);
    kprintf("Cause: ???\n");
    break;
  case 31:
    kprintf("Type: %s\n", isr_exception_messages[31]);
    kprintf("Cause: Reserved\n");
    break;
  default:
    kprintf("Type: Idek how you got here. You fucked up dude, like, fucked up fucked up\n");
    kprintf("Cause: Doing something you really shouldn't have been doing\n");
    break;
  }

  kprintf("\n");
  kprintf("---Registers---\n");
  kprintf("RIP: 0x%016lx (Instruction Pointer)\n", frame->rip);
  kprintf("RSP: 0x%016lx (Stack Pointer)\n", frame->rsp);
  kprintf("RBP: 0x%016lx (Base Pointer)\n", frame->rbp);
  kprintf("RFLAGS: 0x%016lx\n", frame->rflags);
  kprintf("CS: 0x%04lx SS: 0x%04lx\n", frame->cs, frame->ss);

  uint64_t rax, rbx, rcx, rdx, rsi, rdi;
  __asm__ volatile("mov %%rax, %0" : "=m"(rax));
  __asm__ volatile("mov %%rbx, %0" : "=m"(rbx));
  __asm__ volatile("mov %%rcx, %0" : "=m"(rcx));
  __asm__ volatile("mov %%rdx, %0" : "=m"(rdx));
  __asm__ volatile("mov %%rsi, %0" : "=m"(rsi));
  __asm__ volatile("mov %%rdi, %0" : "=m"(rdi));

  kprintf("RAX: 0x%016lx RBX: 0x%016lx\n", rax, rbx);
  kprintf("RCX: 0x%016lx RDX: 0x%016lx\n", rcx, rdx);
  kprintf("RSI: 0x%016lx RDI: 0x%016lx\n", rsi, rdi);

  uint64_t cr0, cr3, cr4;
  __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
  __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
  __asm__ volatile("mov %%cr4, %0" : "=r"(cr4));

  kprintf("\n");
  kprintf("---Control Registers---\n");
  kprintf("CR0: 0x%016lx (System Control)\n", cr0);
  kprintf("CR3: 0x%016lx (Page Directory)\n", cr3);
  kprintf("CR4: 0x%016lx (Extended Features)\n", cr4);

  kprintf("\n");
  kprintf("Code dump (RIP):\n");
  uint8_t *code_ptr = (uint8_t *)frame->rip;
  kprintf("Address: 0x%lx\n", frame->rip);
  kprintf("Bytes: ");

  for (int i = -8; i < 8; i++)
  {
    if (i == 0)
      kprintf("[");
    kprintf("%02x ", code_ptr[i]);
    if (i == 0)
      kprintf("]");
  }
  kprintf("\n");

  kprintf("\n");
  kprintf("Stack dump (RSP):\n");
  uint64_t *stack_ptr = (uint64_t *)frame->rsp;
  kprintf("Stack pointer: 0x%lx\n", frame->rsp);

  for (int i = 0; i < 8; i++)
  {
    kprintf("RSP+%02d: 0x%016lx", i * 8, stack_ptr[i]);
    if (i == 0)
      kprintf(" <- RSP");
    kprintf("\n");
  }

  kprintf("\n");
  kprintf("---Stack Trace---\n");
  uint64_t *rbp_ptr = (uint64_t *)frame->rbp;
  for (int i = 0; i < 5 && rbp_ptr; i++)
  {
    kprintf("Frame %d: RBP=0x%lx, Return=0x%lx\n",
            i, (uint64_t)rbp_ptr, rbp_ptr[1]);
    rbp_ptr = (uint64_t *)rbp_ptr[0];
    if ((uint64_t)rbp_ptr < 0x100000 || (uint64_t)rbp_ptr > 0x1000000)
      break;
  }

  kprintf("\n");
  kprintf("---Memory State---\n");
  kprintf("Kernel stack range: 0x%lx - 0x%lx\n", frame->rsp & ~0xFFF, (frame->rsp & ~0xFFF) + 0x1000);
  kprintf("Code segment: 0x%lx\n", frame->rip & ~0xFFF);

  kprintf("\n");
  kprintf("---System State---\n");
  kprintf("Interrupts: %s\n", (frame->rflags & 0x200) ? "Enabled" : "Disabled");
  kprintf("Privilege level: ring %lu\n", frame->cs & 3);
  kprintf("Paging: %s\n", (cr0 & 0x80000000) ? "Enabled" : "Disabled");

  hcf();
}
