#ifndef KBD_H_
#define KBD_H_

#include <arch/x86_64/arch.h>
#include <arch/x86_64/cpu/isr/isr.h>
#include <arch/x86_64/cpu/pic/pic.h>
#include <arch/x86_64/ps2/ps2.h>
#include <vterm/vterm.h>

void init_keyboard(void);

#endif // KBD_H_
