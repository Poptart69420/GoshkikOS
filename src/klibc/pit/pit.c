#include "pit.h"

void pit_set_reload(uint16_t count) {
    outb(PIT_COMMAND, 0x34);
    outb(PIT_DATA, (uint8_t)count);
    outb(PIT_DATA, (uint8_t)(count >> 8));
}

void pit_set_frequency(uint64_t frequency) {
    uint64_t divisor = PIT_DIVISOR / frequency;
    if (PIT_DIVISOR % frequency > frequency / 2) {
        divisor++;
    }

    pit_set_reload((uint16_t)divisor);
}

void pit_init(void) {
    pit_set_reload(0xffff);
    pit_set_frequency(TIMER_FREQUENCY);
}
