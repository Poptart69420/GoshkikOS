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
  frames[index] &= ~(0x1 << offset);
}
