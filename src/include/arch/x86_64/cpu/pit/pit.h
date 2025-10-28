#ifndef PIT_H_
#define PIT_H_

#include <arch/x86_64/io/io.h>
#include <stdint.h>
#include <vterm/kok.h>

#define PIT_DIVISOR 1193182 // Fancy number thing, I forget why it is this specific number

#define PIT_COMMAND 0x43 // Command port
#define PIT_DATA 0x40    // Data port

#define TIMER_FREQUENCY 1000 // Should probably be moved

uint16_t pit_counter_value(void);
void pit_set_reload_value(uint16_t new_count);
void pit_set_frequency(uint64_t frequency);
void init_pit();

#endif // PIT_H_
