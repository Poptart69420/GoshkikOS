#include <klibc/kmem/string.h>

size_t strlen(const char *s) {
    size_t len = 0;
    while (s[len])
        len++;
    return len;
}

size_t strnlen(const char *s, size_t max) {
    size_t len = 0;
    while (len < max && s[len])
        len++;
    return len;
}

char *stpcpy(char *dest, const char *src) {
    char *p;
    p = mempcpy(dest, src, strlen(src));
    *p = '\0';
    return p;
}

char *stpncpy(char *restrict dest, const char *restrict src, size_t n) {
    size_t len;
    len = strnlen(src, n);
    return memset(memcpy(dest, src, len), 0, n - len);
}

char *strcpy(char *dest, const char *src) {
    stpcpy(dest, src);
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
    stpncpy(dest, src, n);
    return dest;
}

char *strcat(char *dest, const char *src) {
    stpcpy(dest + strlen(dest), src);
    return dest;
}

int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

int strncmp(const char *a, const char *b, size_t n) {
    while (n && *a && (*a == *b)) {
        a++;
        b++;
        n--;
    }
    return n ? (unsigned char)*a - (unsigned char)*b : 0;
}

char *strchr(const char *s, int c) {
    while (*s) {
        if (*s == (char)c)
            return (char *)s;
        s++;
    }
    return NULL;
}

char *strrchr(const char *s, int c) {
    const char *last = NULL;
    while (*s) {
        if (*s == (char)c)
            last = s;
        s++;
    }
    return (char *)last;
}

char *strstr(const char *haystack, const char *needle) {
    if (!*needle)
        return (char *)haystack;

    for (; *haystack; haystack++) {
        const char *h = haystack;
        const char *n = needle;

        while (*h && *n && (*h == *n)) {
            h++;
            n++;
        }

        if (!*n)
            return (char *)haystack;
    }

    return NULL;
}
