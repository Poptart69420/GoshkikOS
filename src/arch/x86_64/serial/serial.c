#include <arch/x86_64/serial/serial.h>

int init_serial(void) {
    vterm_print("Serial...");
    outb(SERIAL_PORT + 1, 0x00);
    outb(SERIAL_PORT + 3, 0x80);
    outb(SERIAL_PORT + 0, 0x03);
    outb(SERIAL_PORT + 1, 0x00);
    outb(SERIAL_PORT + 3, 0x03);
    outb(SERIAL_PORT + 2, 0xC7);
    outb(SERIAL_PORT + 4, 0x0B);
    outb(SERIAL_PORT + 4, 0x1E);
    outb(SERIAL_PORT + 0, 0xAE);

    if (inb(SERIAL_PORT + 0) != 0xAE) {
        kerror("Serial fault");
        return 1;
    }

    outb(SERIAL_PORT + 4, 0x0F);
    kok();
    return 0;
}

void write_serial_char(const char data) {
    while (!(inb(SERIAL_PORT + 5) & 0x20))
        ;
    outb(SERIAL_PORT, data);
}

void write_serial(const char *string) {
    uintmax_t i = 0;
    while (string[i]) {
        write_serial_char(string[i]);
        i++;
    }
}
