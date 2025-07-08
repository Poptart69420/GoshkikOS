#include "pic.h"
#include "../io.h"

void pic_remap(int offset1, int offset2) {
  outb(PIC1_COMMAND, 0x11);
  outb(PIC2_COMMAND, 0x11);
  outb(PIC1_DATA, offset1);
  outb(PIC2_DATA, offset2);
  outb(PIC1_DATA, 0x04);
  outb(PIC2_DATA, 0x02);
  outb(PIC1_DATA, 0x01);
  outb(PIC2_DATA, 0x01);
  outb(PIC1_DATA, 0x0);
  outb(PIC2_DATA, 0x0);
}

void pic_mask_all(void)
  {
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
  }

void pic_mask_irq(uint8_t irq) {
    uint16_t port;
    uint8_t masks;

    if (irq < 8) {
        port = PIC1_DATA;
    }
    else {
        port = PIC2_DATA;
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
        port = PIC1_DATA;
    }
    else {
        port = PIC2_DATA;
        irq -= 8;
    }

    masks = inb(port);
    masks &= ~(1 << irq);
    outb(port, masks);
}
