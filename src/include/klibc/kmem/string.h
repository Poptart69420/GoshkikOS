#ifndef STRING_H_
#define STRING_H_

#include <klibc/kmem/kheap.h>
#include <klibc/kmem/mem.h>

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
char *strdup(const char *str);
char *strtok(char *str, const char *delim);
char *strtok_r(char *str, const char *delim, char **save_ptr);

#endif // STRING_H_
