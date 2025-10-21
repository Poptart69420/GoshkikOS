#ifndef SCHEDULAR_H_
#define SCHEDULAR_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/cpu/gdt/gdt.h>
#include <arch/x86_64/cpu/idt/idt.h>
#include <arch/x86_64/cpu/timer/timer.h>
#include <arch/x86_64/selectors/selectors.h>
#include <global/global.h>
#include <scheduling/threadding.h>
#include <stdint.h>

void schedule_tick(fault_frame_t *frame);
void remove_from_queue(thread_t *target);
void add_to_queue(thread_t *thread);

#endif // SHEDULAR_H_
