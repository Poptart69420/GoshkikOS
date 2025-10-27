#ifndef GDT_H_
#define GDT_H_

#include <global/global.h>
#include <klibc/kmem/mem.h>
#include <scheduling/spinlock.h>
#include <stddef.h>
#include <stdint.h>
#include <vterm/kerror.h>
#include <vterm/kok.h>
#include <vterm/vterm.h>

struct tss_desc
{
  uint16_t length;
  uint16_t base_low;
  uint8_t base_mid;
  uint8_t flags;
  uint8_t flags2;
  uint8_t base_high;
  uint32_t base_upper32;
  uint32_t reserved;
} __attribute__((packed));

typedef struct
{
  uint32_t reserved;
  uint64_t rsp0;
  uint64_t rsp1;
  uint64_t rsp2;
  uint64_t reserved1;
  uint64_t ist1;
  uint64_t ist2;
  uint64_t ist3;
  uint64_t ist4;
  uint64_t ist5;
  uint64_t ist6;
  uint64_t ist7;
  uint64_t reserved2;
  uint16_t reserved3;
  uint16_t base_iomap;
} __attribute__((packed)) tss_t;

struct gdt_desc
{
  uint16_t limit;
  uint16_t base_low;
  uint8_t base_mid;
  uint8_t access;
  uint8_t granularity;
  uint8_t base_high;

} __attribute__((packed));

struct gdt_ptr
{
  uint16_t limit;
  uint64_t ptr;
} __attribute__((packed));

struct gdtr
{
  struct gdt_desc entries[5];
  struct tss_desc tss;
} __attribute__((packed));

extern tss_t tss;

void init_gdt(void);
void set_kernel_stack(uintptr_t stack);

#endif // GDT_H_
