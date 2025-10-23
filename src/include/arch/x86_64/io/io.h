#ifndef IO_H_
#define IO_H_

#include <stdint.h>

static inline uint8_t inb(uint16_t port)
{
  uint8_t data;
  __asm__ volatile("in al, dx" : "=a"(data) : "d"(port));
  return data;
}

static inline void outb(uint16_t port, uint8_t data)
{
  __asm__ volatile("out dx, al" : : "a"(data), "d"(port));
}

static inline uint16_t inw(uint16_t port)
{
  uint16_t data;
  __asm__ volatile("in ax, dx" : "=a"(data) : "d"(port));
  return data;
}

static inline void outw(uint16_t port, uint16_t data)
{
  __asm__ volatile("out dx, ax" : : "a"(data), "d"(port));
}

static inline uint32_t inl(uint16_t port)
{
  uint32_t data;
  __asm__ volatile("in eax, dx" : "=a"(data) : "d"(port));
  return data;
}

static inline void outl(uint16_t port, uint32_t data)
{
  __asm__ volatile("out dx, eax" : : "a"(data), "d"(port));
}

#endif // IO_H_
