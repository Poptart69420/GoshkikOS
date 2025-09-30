#ifndef GDT_H_
#define GDT_H_

#include <stddef.h>
#include <stdint.h>

#include "../drivers/vterm/kerror.h"
#include "../drivers/vterm/kok.h"
#include "../drivers/vterm/vterm.h"
#include "../include/selectors.h"
#include "../mem/mem.h"
#include "../scheduling/spinlock.h"

struct tss_desc {
    uint16_t length;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t flags;
    uint8_t flags2;
    uint8_t base_high;
    uint32_t base_upper32;
    uint32_t reserved;
} __attribute__((packed));

struct tss {
    uint32_t reserved;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved2;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved3;
    uint16_t reserved4;
    uint16_t base_iomap;
} __attribute__((packed));

struct gdt_desc {
    uint16_t limit;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;

} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint64_t ptr;
} __attribute__((packed));

struct gdtr {
    struct gdt_desc entries[5];
    struct tss_desc tss;
} __attribute__((packed));

void gdt_setup(void);
void gdt_init(void);

#endif // GDT_H_
