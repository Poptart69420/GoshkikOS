#ifndef PIC_H_
#define PIC_H_

#include "../include/io.h"
#include <stdint.h>

#define COM1_PORT 0x3F8

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

void pic_remap(int offset1, int offset2);
void pic_mask_irq(uint8_t irq);
void pic_unmask_irq(uint8_t irq);
void pic_mask_all(void);
void pic_send_eoi(uint8_t irq);

#endif // PIC_H_
