#ifndef PIT_H_
#define PIT_H_

#include <arch/x86_64/io/io.h>
#include <stdint.h>

#define PIT_DIVISOR 1193182

#define PIT_COMMAND 0x43
#define PIT_DATA 0x40

#define TIMER_FREQUENCY 1000

uint16_t pit_counter_value(void);
void pit_set_reload_value(uint16_t new_count);
void pit_set_frequency(uint64_t frequency);
void pit_init();

#endif // PIT_H_
