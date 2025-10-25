#include <scheduling/syscall.h>

static syscall_entry_t syscall_table[MAX_SYS_CALLS];

void syscall_handler(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6)
{
  int64_t result;
  if (syscall_num >= MAX_SYS_CALLS || syscall_table[syscall_num].handler == NULL)
  {
    result = -1;
  }
  else
  {
    result = syscall_table[syscall_num].handler(arg1, arg2, arg3, arg4, arg5, arg6);
  }
  __asm__ volatile("mov rax, %0" : : "r"(result) : "rax");
}

void init_syscalls(void)
{
  kprintf("System Calls...");
  for (int i = 0; i < MAX_SYS_CALLS; ++i)
  {
    syscall_table[i].handler = NULL;
    syscall_table[i].name = "";
    syscall_table[i].arg_num = 0;
  }

  idt_set_entry(0x80, (uint64_t)syscall_entry, KERNEL_CODE64, 0xEE);
  kok();
}
