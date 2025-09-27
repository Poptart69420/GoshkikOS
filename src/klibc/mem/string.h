#ifndef STRING_H_
#define STRING_H_

#include "mem.h"
#include <stddef.h>
#include <stdint.h>

size_t strlen(const char *s);
size_t strnlen(const char *s, size_t max);
char *strcpy(char *dest, const char *src);
char *stpcpy(char *dest, const char *src);
char *stpncpy(char *restrict dest, const char *restrict src, size_t n);
char *strncpy(char *dest, const char *src, size_t n);
char *strcat(char *dest, const char *src);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *haystack, const char *needle);

#endif // STRING_H_
