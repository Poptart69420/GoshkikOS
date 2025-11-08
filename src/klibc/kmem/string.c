#include <klibc/kmem/string.h>

size_t strlen(const char *s)
{
  size_t len = 0;
  while (s[len])
    len++;
  return len;
}

size_t strnlen(const char *s, size_t max)
{
  size_t len = 0;
  while (len < max && s[len])
    len++;
  return len;
}

char *stpcpy(char *restrict dest, const char *restrict src)
{
  char *p;
  p = mempcpy(dest, src, strlen(src));
  *p = '\0';
  return p;
}

char *stpncpy(char *restrict dest, const char *restrict src, size_t n)
{
  size_t len;
  len = strnlen(src, n);
  return memset(memcpy(dest, src, len), 0, n - len);
}

char *strcpy(char *dest, const char *src)
{
  stpcpy(dest, src);
  return dest;
}

char *strncpy(char *restrict dest, const char *restrict src, size_t n)
{
  stpncpy(dest, src, n);
  return dest;
}

char *strcat(char *restrict dest, const char *restrict src)
{
  stpcpy(dest + strlen(dest), src);
  return dest;
}

int strcmp(const char *a, const char *b)
{
  while (*a && (*a == *b))
  {
    a++;
    b++;
  }
  return (unsigned char)*a - (unsigned char)*b;
}

int strncmp(const char *restrict a, const char *restrict b, size_t n)
{
  while (n && *a && (*a == *b))
  {
    a++;
    b++;
    n--;
  }
  return n ? (unsigned char)*a - (unsigned char)*b : 0;
}

char *strchr(const char *s, int c)
{
  while (*s)
  {
    if (*s == (char)c)
      return (char *)s;
    s++;
  }
  return NULL;
}

char *strrchr(const char *s, int c)
{
  const char *last = NULL;
  while (*s)
  {
    if (*s == (char)c)
      last = s;
    s++;
  }
  return (char *)last;
}

char *strstr(const char *haystack, const char *needle)
{
  if (!*needle)
    return (char *)haystack;

  for (; *haystack; haystack++)
  {
    const char *h = haystack;
    const char *n = needle;

    while (*h && *n && (*h == *n))
    {
      h++;
      n++;
    }

    if (!*n)
      return (char *)haystack;
  }

  return NULL;
}

char *strdup(const char *str)
{
  size_t str_size = strlen(str) + 1;
  char *new_str = kmalloc(str_size);
  return strcpy(new_str, str);
}

char *strtok_r(char *str, const char *delim, char **save_ptr)
{
  if (str)
  {
    *save_ptr = str;
    return NULL;
  }

  while (strchr(delim, **save_ptr) && save_ptr)
    (*save_ptr)++;
  char *ret = *save_ptr;
  while (!strchr(delim, **save_ptr) && **save_ptr)
    (*save_ptr)++;

  if (!*ret)
    return NULL;
  return ret;
}

char *strtok(char *str, const char *delim)
{
  static char *ptr;
  return strtok_r(str, delim, &ptr);
}

size_t strlcpy(char *restrict dst, const char *restrict src, size_t size)
{
  const char *s = src;
  size_t n = size;

  if (n != 0)
  {
    while (--n != 0)
    {
      if ((*dst++ = *s++) == '\0')
        break;
    }
  }

  if (n == 0)
  {
    if (size != 0)
      *dst = '\0';
    while (*s++)
      ;
  }

  return (size_t)(s - src - 1);
}

size_t strlcat(char *restrict dst, const char *restrict src, size_t size)
{
  size_t dlen = strnlen(dst, size);
  size_t slen = strlen(src);

  if (dlen == size)
    return size + slen;

  size_t copy = size - dlen - 1;
  if (copy > slen)
    copy = slen;

  memcpy(dst + dlen, src, copy);
  dst[dlen + copy] = '\0';

  return dlen + slen;
}
