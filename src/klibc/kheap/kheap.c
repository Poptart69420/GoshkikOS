#include "kheap.h"

extern uint32_t end;
uint32_t placement_address;

uint32_t kmalloc_init(uint32_t size, int align, uint32_t *physical_address)
{
  placement_address = (uint32_t)&end;

  if (align == 1 && (placement_address & 0xFFFFF000)) {
    placement_address &= 0xFFFFF000;
    placement_address += 0x1000;
  }

  if (physical_address) {
    *physical_address = placement_address;
  }

  uint32_t tmp = placement_address;
  placement_address += size;
  return tmp;
}

uint32_t kmalloc_a(uint32_t size)
{
  return kmalloc_init(size, 1, 0);
}

uint32_t kmalloc_p(uint32_t size, uint32_t *physical_address)
{
  return kmalloc_init(size, 0, physical_address);
}

uint32_t kmalloc_ap(uint32_t size, uint32_t *physical_address)
{
  return kmalloc_init(size, 1, physical_address);
}

uint32_t kmalloc(uint32_t size)
{
  return kmalloc_init(size, 0, 0);
}
