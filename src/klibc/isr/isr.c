#include "isr.h"

static handlers_t handlers[IDT_ENTRIES] = {NULL};

void isr_register(int isr, handlers_t handler) { handlers[isr] = handler; }

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

void isr_install(void) {

    for (int i = 0; i < IDT_ENTRIES; ++i) {
        uint8_t ist = 0;
        uint8_t attr = IDT_INT_KERNEL;

        if (i == 14) {
            ist = 2;
        }

        if (i == 0x80) {
            attr = IDT_INT_USER;
        }

        idt_set_gate(i, isr_stub_table[i], ist, attr);
    }

    idt_reload();
}

void isr_handler(registers_t *reg) {
    if (reg->cs & 0x3) {
        __asm__ volatile("swapgs" ::: "memory");
    }

    if (reg->isr < IDT_ENTRIES && handlers[reg->isr] != NULL) {
        handlers[reg->isr](reg);
    }

    if (reg->isr < 32) {
        if (reg->cs & 0x3) {
            vterm_print("\n");
            vterm_print("-----EXCEPTION-----\n");
            vterm_print(isr_exception_messages[reg->isr]);
            // do something
        } else {
            vterm_print("\n");
            vterm_print("-----KERNEL EXCEPTION-----\n");
            vterm_print(isr_exception_messages[reg->isr]);

            for (;;)
                __asm__ volatile("hlt");

            // do something
        }
    }

    if (reg->cs & 0x3) {
        __asm__ volatile("swapgs" ::: "memory");
    }
}
