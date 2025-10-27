#ifndef TIMER_H_
#define TIMER_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/asm/asm.h>
#include <arch/x86_64/cpu/isr/isr.h>
#include <arch/x86_64/cpu/pic/pic.h>
#include <klibc/printf.h>
#include <stdatomic.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

extern atomic_uint_fast64_t monotonic_ms;

struct system_time
{
  uint32_t milliseconds; // 0–999
  uint8_t seconds;       // 0–59
  uint8_t minutes;       // 0–59
  uint8_t hours;         // 0–23
  uint8_t _pad0;
  uint32_t days;
} __attribute__((packed, aligned(4)));

void timer_handler(context_t *context);
void init_system_clock(void);
void get_system_time(struct system_time *time);
uint64_t uptime_ms(void);
uint32_t uptime_seconds(void);
uint32_t uptime_minutes(void);
int time_after(uint64_t a, uint64_t b);
int time_before(uint64_t a, uint64_t b);

#endif // TIMER_H_
