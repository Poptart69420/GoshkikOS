#ifndef ARCH_H_
#define ARCH_H_

#include <arch/x86_64/cpu/gdt/gdt.h>
#include <arch/x86_64/cpu/idt/idt.h>
#include <stdint.h>

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
} __attribute__((packed)) fault_frame_t;

#define ARG0_REG(frame) (frame).rax
#define ARG1_REG(frame) (frame).rdi
#define ARG2_REG(frame) (frame).rsi
#define ARG3_REG(frame) (frame).rdx
#define ARG4_REG(frame) (frame).rcx
#define ARG5_REG(frame) (frame).r8
#define RET_REG(frame) (frame).rax
#define SP_REG(frame) (frame).rsp

#endif // ARCH_H_
