#include "hcf.h"

void hcf(void) {
    for (;;) {
        __asm__ volatile("hlt");
    }
}
