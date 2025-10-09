#include <klibc/assert.h>

static char *utoa(unsigned int n, char *buf)
{
  char *p = buf;
  char *q = buf;
  do
  {
    *q++ = '0' + (n % 10);
    n /= 10;
  } while (n);
  *q-- = '\0';
  while (p < q)
  {
    char tmp = *p;
    *p++ = *q;
    *q-- = tmp;
  }
  return buf;
}

__attribute__((noreturn)) void __assert(const char *expr, const char *file, int line, const char *func)
{
  kerror("Assertion failed:");
  kerror(expr);
  kerror("Function:");
  kerror(func ? func : "<unknown>");
  kerror("File:");
  kerror(file);
  char buf[16];
  kerror("Line:");
  kerror(utoa(line, buf));
  hcf();
  __builtin_unreachable();
}
