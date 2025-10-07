#ifndef IO_H_
#define IO_H_

#include <stdint.h>

static inline uint8_t inb(uint16_t port) {
  uint8_t data;
  __asm__ volatile("inb %%dx, %%al" : "=a"(data) : "d"(port));
  return data;
}

static inline void outb(uint16_t port, uint8_t data) {
  __asm__ volatile("outb %%al, %%dx" : : "a"(data), "d"(port));
}

static inline uint16_t inw(uint16_t port) {
  uint16_t data;
  __asm__ volatile("inw %%dx, %%ax" : "=a"(data) : "d"(port));
  return data;
}

static inline void outw(uint16_t port, uint16_t data) {
  __asm__ volatile("outw %%ax, %%dx" : : "a"(data), "d"(port));
}

static inline void inl(uint16_t port) {
  uint32_t data;
  __asm__ volatile("inl %%dx, %%eax" : "=a"(data) : "d"(port));
}

static inline void outl(uint16_t port, uint32_t data) {
  __asm__ volatile("outl %%eax, %%dx" : : "d"(port), "a"(data));
}

#endif // IO_H_
