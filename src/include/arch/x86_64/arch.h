#ifndef ARCH_H_
#define ARCH_H_

#include <arch/x86_64/cpu/gdt/gdt.h>
#include <arch/x86_64/cpu/idt/idt.h>
#include <stdint.h>

typedef struct
{
  uint64_t gs;
  uint64_t fs;
  uint64_t es;
  uint64_t ds;
  uint64_t cr2;
  uint64_t cr3;
  uint64_t rax;
  uint64_t rbx;
  uint64_t rcx;
  uint64_t rdx;
  uint64_t rsi;
  uint64_t rdi;
  uint64_t rbp;
  uint64_t r8;
  uint64_t r9;
  uint64_t r10;
  uint64_t r11;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
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
