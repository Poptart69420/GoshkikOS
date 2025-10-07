// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef _WCHAR_H
#define _WCHAR_H

#include <stddef.h>
#include <stdio.h>

#ifndef __WINT_TYPE__
#define __WINT_TYPE__ signed int
#endif

typedef __WINT_TYPE__ wint_t;

#define WEOF EOF

// maybee change size of this for UTF 7 ??
typedef int mbstate_t;

wint_t btowc(int c);
int mbrtowc(wchar_t *dest, const char *s, size_t n, mbstate_t *ps);
int mbtowc(wchar_t *dest, const char *s, size_t n);
size_t mbstowcs(wchar_t *dest, const char *src, size_t n);
int wcrtomb(char *s, wchar_t wc, mbstate_t *ps);
int wctomb(char *s, wchar_t wc);
int mbsinit(const mbstate_t *ps);

wint_t fgetwc(FILE *stream);
wint_t getwc(FILE *stream);
wint_t getwchar(void);
wchar_t *fgetws(wchar_t *ws, int n, FILE *stream);
wint_t fputwc(wchar_t wc, FILE *stream);
wint_t putwc(wchar_t wc, FILE *stream);
wint_t putwchar(wchar_t wc);
int fputws(const wchar_t *ws, FILE *stream);

wchar_t *wcscat(wchar_t *dest, const wchar_t *src);
int wcscmp(const wchar_t *ws1, const wchar_t *ws2);
wchar_t *wcschr(const wchar_t *ws, wchar_t wc);
wchar_t *wcsstr(const wchar_t *ws1, const wchar_t *ws2);
wchar_t *wcscpy(wchar_t *dest, const wchar_t *src);
size_t wcslen(const wchar_t *ws);
wchar_t *wcspbrk(const wchar_t *wcs, const wchar_t *search);

wchar_t *wmemchr(const wchar_t *, wchar_t, size_t);
int wmemcmp(const wchar_t *, const wchar_t *, size_t);
wchar_t *wmemcpy(wchar_t *, const wchar_t *, size_t);
wchar_t *wmemmove(wchar_t *, const wchar_t *, size_t);
wchar_t *wmemset(wchar_t *, wchar_t, size_t);

// ctype
wint_t towupper(wint_t wc);
wint_t towlower(wint_t wc);
int iswupper(wint_t wc);
int iswlower(wint_t wc);
int iswcntrl(wint_t wc);

#endif

_END_C_HEADER
