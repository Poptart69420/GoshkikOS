#ifndef ARCH_H_
#define ARCH_H_

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

#endif // ARCH_H_
