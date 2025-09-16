#include "stdlib.h"

void exit(int status)
{
  (void) status; // Suppress warning

  __asm__ volatile ("cli");
  for (;;) {
    __asm__ volatile ("hlt");
  }
}
