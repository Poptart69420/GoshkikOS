// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <sys/types.h>

// maybee should be equal to PATH_MAX from limits.h ???
#define FILENAME_MAX 256

struct _FILE
{
  struct _FILE *next;
  struct _FILE *prev;
  char *buf;
  size_t bufsize;
  size_t usedsize;
  unsigned long errno;
  int fd;
  int eof;
  int unget;
  int buftype;
};
typedef struct _FILE FILE;

typedef size_t fpos_t;

int sprintf(char *str, const char *fmt, ...);
int vsprintf(char *buf, const char *fmt, va_list args);
int snprintf(char *str, size_t maxlen, const char *fmt, ...);
int vsnprintf(char *buf, size_t maxlen, const char *fmt, va_list args);
int fprintf(FILE *stream, const char *fmt, ...);
int vfprintf(FILE *stream, const char *fmt, va_list args);
int printf(const char *fmt, ...);
int vprintf(const char *fmt, va_list args);

int sscanf(const char *buf, const char *fmt, ...);
int vsscanf(const char *buf, const char *fmt, va_list args);
int fscanf(FILE *stream, const char *fmt, ...);
int vfscanf(FILE *stream, const char *fmt, va_list args);

FILE *fopen(const char *path, const char *mode);
FILE *freopen(const char *pathname, const char *mode, FILE *stream);
FILE *fdopen(int handle, const char *type);
FILE *tmpfile(void);
int fclose(FILE *stream);
size_t fread(void *ptr, size_t size, size_t n, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t n, FILE *stream);

int fileno(FILE *stream);

// seek func
int fseek(FILE *stream, long int offset, int origin);
long int ftell(FILE *stream);
void rewind(FILE *stream);

// put/get
int fgetc(FILE *stream);
int getc(FILE *stream);
int getchar(void);
char *fgets(char *string, int n, FILE *stream);
char *gets(char *buffer);
int fputc(int c, FILE *stream);
int putc(int c, FILE *stream);
int putchar(int c);
int fputs(const char *string, FILE *stream);
int puts(const char *string);

void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
void perror(const char *string);

int ungetc(int c, FILE *stream);

#define _IONBF 0
#define _IOFBF 1
#define _IOLBF 2

int fflush(FILE *stream);
int setvbuf(FILE *stream, char *buf, int type, size_t size);

int rename(const char *oldpath, const char *newpath);
int remove(const char *pathname);

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

#define PRINTF_MAX 1024

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif
#ifndef SEEK_END
#define SEEK_END 2
#endif

#ifndef NULL
#define NULL (void *)0
#endif

#ifndef BUFSIZ
#define BUFSIZ 4096
#endif

#define EOF -1

#endif

_END_C_HEADER
