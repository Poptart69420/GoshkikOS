#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/cpu/gdt/gdt.h>
#include <arch/x86_64/cpu/idt/idt.h>
#include <arch/x86_64/cpu/timer/timer.h>
#include <arch/x86_64/mem/vmm/vmm.h>
#include <arch/x86_64/selectors/selectors.h>
#include <global/global.h>
#include <klibc/cred/cred.h>
#include <klibc/kmem/list.h>
#include <klibc/types.h>
#include <scheduling/mutex.h>
#include <stdint.h>
#include <sys/time.h>

#endif // SHEDULER_H_
