#ifndef MEM_H_
#define MEM_H_

#include <limine/limine.h>
#include <limits.h>
#include <stddef.h>

void *memcpy(void *restrict dest, const void *restrict src, size_t n);
void *mempcpy(void *restrict dest, const void *restrict src, size_t len);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

#endif // MEM_H_
