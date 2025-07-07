#include "pic.h"
#include "../io.h"

void pic_remap(int offset1, int offset2) {
  outb(0x20, 0x11);
  outb(0xA0, 0x11);
  outb(0x21, offset1);
  outb(0xA1, offset2);
  outb(0x21, 0x04);
  outb(0xA1, 0x02);
  outb(0x21, 0x01);
  outb(0xA1, 0x01);
  outb(0x21, 0x0);
  outb(0xA1, 0x0);
}

void pic_mask_all(void)
  {
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
  }

void pic_mask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t masks;

    if (irq < 8) {
        port = 0x21;
    }
    else {
        port = 0xA1;
        irq -= 8;
    }

    masks = inb(port);
    masks |= (1 << irq);
    outb(port, masks);
}

void pic_unmask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t masks;

    if (irq < 8) {
        port = 0x21;
    }
    else {
        port = 0xA1;
        irq -= 8;
    }

    masks = inb(port);
    masks &= ~(1 << irq);
    outb(port, masks);
}
