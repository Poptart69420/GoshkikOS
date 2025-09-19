#ifndef STRING_H_
#define STRING_H_

#include <stdint.h>
#include <stddef.h>

size_t strlen(const char *s);
size_t strnlen(const char *s, size_t max);
void strcpy(char *dest, const char *src);
void strncpy(char *dest, const char *src, size_t n);
void strcat(char *dest, const char *src);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, size_t n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *haystack, const char *needle);


#endif // STRING_H_
