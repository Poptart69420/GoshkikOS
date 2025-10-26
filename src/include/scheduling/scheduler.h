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
#include <scheduling/thread.h>
#include <stdint.h>
#include <sys/time.h>

extern void context_switch(fault_frame_t *old_ctx, fault_frame_t *new_ctx);

void scheduler_tick(fault_frame_t *frame);
void add_to_ready_queue(thread_t *t);

#endif // SHEDULER_H_
