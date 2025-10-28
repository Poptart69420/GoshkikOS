#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/cpu/gdt/gdt.h>
#include <arch/x86_64/cpu/idt/idt.h>
#include <arch/x86_64/cpu/timer/timer.h>
#include <arch/x86_64/mem/vmm/vmm.h>
#include <klibc/cred/cred.h>
#include <klibc/types.h>
#include <scheduling/mutex.h>
#include <scheduling/thread.h>
#include <stdint.h>
#include <sys/time.h>

extern void context_switch(context_t *old_ctx, context_t *new_ctx, int is_ring_3);

void scheduler_tick(context_t *context);
void add_to_ready_queue(thread_t *t);

#endif // SHEDULER_H_
