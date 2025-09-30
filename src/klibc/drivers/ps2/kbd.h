#ifndef KBD_H_
#define KBD_H_

#include "../../arch/x86_64/arch.h"
#include "../../isr/isr.h"
#include "../../pic/pic.h"
#include "../vterm/vterm.h"
#include "ps2.h"

void init_keyboard(void);

#endif // KBD_H_
