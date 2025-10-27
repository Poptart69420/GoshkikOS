#include <arch/x86_64/cpu/timer/timer.h>
#include <kernel.h>

atomic_uint_fast64_t monotonic_ms = ATOMIC_VAR_INIT(0);

void init_system_clock(void)
{
  kprintf("System Clock...");
  kernel->current_time.milliseconds = 0;
  kernel->current_time.seconds = 0;
  kernel->current_time.minutes = 0;
  kernel->current_time.hours = 0;
  kernel->current_time.days = 0;
  atomic_store_explicit(&monotonic_ms, 0, memory_order_relaxed);
  kok();
}

void timer_handler(context_t *context)
{
  (void)context;

  atomic_fetch_add_explicit(&monotonic_ms, 1, memory_order_relaxed);

  /* use dot access */
  if (++kernel->current_time.milliseconds < 1000)
    return;
  kernel->current_time.milliseconds = 0;

  if (++kernel->current_time.seconds < 60)
    return;
  kernel->current_time.seconds = 0;

  if (++kernel->current_time.minutes < 60)
    return;
  kernel->current_time.minutes = 0;

  if (++kernel->current_time.hours < 24)
    return;
  kernel->current_time.hours = 0;

  kernel->current_time.days++;
}

void get_system_time(struct system_time *time)
{
  memcpy(time, &kernel->current_time, sizeof(*time));
}

uint64_t uptime_ms(void)
{
  return (uint64_t)atomic_load_explicit(&monotonic_ms, memory_order_relaxed);
}

uint32_t uptime_seconds(void)
{
  return (uint32_t)(uptime_ms() / 1000ULL);
}

uint32_t uptime_minutes(void)
{
  return (uint32_t)(uptime_ms() / 60000ULL);
}

int time_after(uint64_t a, uint64_t b)
{
  return (int64_t)(b - a) < 0;
}

int time_before(uint64_t a, uint64_t b)
{
  return (int64_t)(a - b) < 0;
}
