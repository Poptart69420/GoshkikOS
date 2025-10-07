#include <arch/x86_64/asm/hcf.h>

void hcf(void) {
  disable_interrupt();
  halt();
}
