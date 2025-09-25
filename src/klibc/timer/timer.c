#include "timer.h"

void timer_handler(registers_t *reg) {
    (void)reg; // Suppress warning

    static int ticks = 0;
    ticks++;
    pic_send_eoi(0);

    (void)ticks; // Supress warning
}

void init_timer(void) { isr_register(0x20, timer_handler); }
