#ifndef SYSCALL_H_
#define SYSCALL_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/asm/asm.h>
#include <arch/x86_64/cpu/idt/idt.h>
#include <arch/x86_64/selectors/selectors.h>
#include <stdint.h>

#define MAX_SYS_CALLS 420

typedef int64_t (*syscall_handler_t)(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6);

typedef struct
{
  syscall_handler_t handler;
  const char *name;
  int arg_num;
} syscall_entry_t;

extern void syscall_entry(void);

void syscall_handler(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6);
void init_syscalls(void);

#endif // SYSCALL_H_
