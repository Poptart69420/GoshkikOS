#include "hcf.h"

static void hcf(void)
{
  for (;;) {
    __asm__ volatile ("hlt");
  }
}
