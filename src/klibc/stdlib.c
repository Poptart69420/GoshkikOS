#include "stdlib.h"

void exit(int status)
{
  __asm__ volatile ("cli");
  for (;;) {
    __asm__ volatile ("hlt");
  }
}
