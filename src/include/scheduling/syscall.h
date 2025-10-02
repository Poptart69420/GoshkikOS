#ifndef SYSCALL_H_
#define SYSCALL_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/asm/asm.h>
#include <stdint.h>

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

#endif // SYSCALL_H_
