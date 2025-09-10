#ifndef LIMINE_REQUESTS_H_
#define LIMINE_REQUESTS_H_

#include "../../limine.h"

extern volatile uint64_t limine_base_revision[3];

extern volatile struct limine_framebuffer_request framebuffer_request;
extern volatile struct limine_hhdm_request        hhdm_request;
extern volatile struct limine_memmap_request      memmap_request;

#endif // LIMINE_REQUESTS_H_
