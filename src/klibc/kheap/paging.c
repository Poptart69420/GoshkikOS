#include "paging.h"

#define INDEX_FROM_BIT(a) (a/(8 * 4))
#define OFFSET_FROM_BIT(a) (a%(8 * 4))

page_directory_t *kernel_directory = 0;
page_directory_t *current_directory = 0;

uint32_t *frames;
uint32_t nframes;

extern uint32_t placement_address;

static void set_frame(uint32_t frame_address)
{
  uint32_t frame = frame_address / 0x1000;
  uint32_t index = INDEX_FROM_BIT(frame);
  uint32_t offset = OFFSET_FROM_BIT(frame);
  frames[index] |= ~(0x1 << offset);
}

static void clear_frame(uint32_t frame_address)
{
  uint32_t frame = frame_address / 0x1000;
  uint32_t index = INDEX_FROM_BIT(frame);
  uint32_t offset = OFFSET_FROM_BIT(frame);
  frames[index] &= ~(0x1 << offset);
}

static uint32_t test_frame(uint32_t frame_address)
{
  uint32_t frame = frame_address / 0x1000;
  uint32_t index = INDEX_FROM_BIT(frame);
  uint32_t offset = OFFSET_FROM_BIT(frame);
  return (frames[index] & (0x1 << offset));
}

static uint32_t first_frame(void)
{
  for (uint32_t i = 0; i < INDEX_FROM_BIT(nframes); ++i) {
    if (frames[i] == 0xFFFFFFFF) {
      continue;
    }

    uint32_t free_bit_offset = __builtin_ctz(~frames[i]);

    return i * 32 + free_bit_offset;
  }
  return (uint32_t)-1;

}

void alloc_frame(page_t *page, int kernel, int writable)
{
  if (page->frame != 0) {
    return;
  }

  uint32_t index = first_frame();
  if (index == (uint32_t)-1) {
    for (;;)
    __asm__ volatile ("hlt");
  }

  set_frame(index * 0x1000);
  page->present = 1;
  page->rw = (writable) ? 1:0;
  page->user = (kernel) ? 0:1;
  page->frame = index;

}

void free_frame(page_t *page)
{
  uint32_t frame;
  if (!(frame = page->frame)) {
    return;
  }

  clear_frame(frame);
  page->frame = 0x0;

}
