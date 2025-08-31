#ifndef PS2_H_
#define PS2_H_

#include <stdint.h>
#include <stdbool.h>
#include "../../mem.h"
#include "../../include/io.h"
#include "../vterm/vterm.h"
#include "../../include/registers.h"

#define PS2_DATA        0x60
#define PS2_COMMAND     0x64

#define PS2_ENABLE_P1   0xAE
#define PS2_ENABLE_P2   0xA8
#define PS2_DISABLE_P1  0xAD
#define PS2_DISABLE_P2  0xA7

uint8_t ps2_read(void);
void ps2_write(uint16_t port, uint8_t value);
uint8_t get_ps2_config(void);
void write_ps2_config(uint8_t value);
uint64_t ps2_entry(void);

#endif // PS2_H_
