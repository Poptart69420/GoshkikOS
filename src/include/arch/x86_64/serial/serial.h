#ifndef SERIAL_H_
#define SERIAL_H_

#include <arch/x86_64/io/io.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

#define SERIAL_PORT 0x3F8 // This is the serial port. Maybe define somewhere else?

int init_serial(void);
void write_serial_char(const char data);
void write_serial(const char *string);

#endif // SERIAL_H_
