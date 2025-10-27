#ifndef ARCH_H_
#define ARCH_H_

#include <arch/x86_64/cpu/gdt/gdt.h>
#include <arch/x86_64/cpu/idt/idt.h>
#include <stdint.h>

#define KERNEL_CODE64 0x08
#define KERNEL_DATA64 0x10
#define USER_CODE64 0x1B
#define USER_DATA64 0x23
#define TSS 0x28

typedef struct
{
  uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
  uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
  uint64_t int_no;
  uint64_t err_code;
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
} __attribute__((packed)) context_t;

#endif // ARCH_H_
