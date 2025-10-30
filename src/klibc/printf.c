#include <klibc/printf.h>
#include <scheduling/spinlock.h>

static spinlock_t kprintf_spinlock;

void kputc(char c)
{
  write_serial_char(c);
  vterm_putc(c);
}

void kprints(const char *str)
{
  while (*str)
    kputc(*str++);
}

void kprintnp(uint64_t num, int base, int uppercase, int width, char pad)
{
  char buffer[32];

  char *digits = uppercase ? DIGITS_UPPERCASE : DIGITS_LOWERCASE;
  int i = 0;

  if (num == 0)
  {
    buffer[++i] = '0';
  }
  else
  {
    while (num > 0)
    {
      buffer[++i] = digits[num % base];
      num /= base;
    }
  }

  while (i < width)
  {
    buffer[++i] = pad;
  }

  while (--i >= 0)
    kputc(buffer[i]);
}

void kprintns(int64_t num, int base)
{
  if (num < 0)
  {
    kputc('-');
    num = -num;
  }

  kprintnp((uint64_t)num, base, 0, 0, ' ');
}

int kprintf(const char *format, ...)
{
  spinlock_acquire(&kprintf_spinlock);

  __builtin_va_list args;
  __builtin_va_start(args, format);

  int count = 0;

  while (*format)
  {
    if (*format == '%')
    {
      format++;

      int width = 0;
      char pad = ' ';
      int uppercase = 0;
      int is_long = 0;

      if (*format == '0')
      {
        pad = '0';
        format++;
      }

      while (*format >= '0' && *format <= '9')
      {
        width = width * 10 + (*format - '0');
        format++;
      }

      if (*format == 'l')
      {
        is_long = 1;
        format++;
      }

      switch (*format)
      {
      case 'c':
      {
        char c = (char)__builtin_va_arg(args, int);
        kputc(c);
        count++;
        break;
      }
      case 's':
      {
        const char *str = __builtin_va_arg(args, const char *);
        if (!str)
          str = "(null)";
        kprints(str);
        count += strlen(str);
        break;
      }
      case 'd':
      case 'i':
      {
        if (is_long)
          kprintns(__builtin_va_arg(args, int64_t), 10);
        else
          kprintns(__builtin_va_arg(args, int), 10);
        count++;
        break;
      }
      case 'u':
      {
        if (is_long)
          kprintnp(__builtin_va_arg(args, uint64_t), 10, 0, width, pad);
        else
          kprintnp(__builtin_va_arg(args, unsigned int), 10, 0, width, pad);
        count++;
        break;
      }
      case 'x':
      {
        if (is_long)
          kprintnp(__builtin_va_arg(args, uint64_t), 16, 0, width, pad);
        else
          kprintnp(__builtin_va_arg(args, unsigned int), 16, 0, width, pad);
        count++;
        break;
      }
      case 'X':
      {
        uppercase = 1;
        if (is_long)
          kprintnp(__builtin_va_arg(args, uint64_t), 16, uppercase, width, pad);
        else
          kprintnp(__builtin_va_arg(args, unsigned int), 16, uppercase, width, pad);
        count++;
        break;
      }
      case 'p':
      {
        void *ptr = __builtin_va_arg(args, void *);
        kprints("0x");
        kprintnp((uint64_t)ptr, 16, 0, sizeof(void *) * 2, '0');
        count++;
        break;
      }
      case '%':
      {
        kputc('%');
        count++;
        break;
      }
      default:
      {
        kputc('%');
        kputc(*format);
        count += 2;
        break;
      }
      }
    }
    else
    {
      kputc(*format);
      count++;
    }
    format++;
  }

  __builtin_va_end(args);

  spinlock_release(&kprintf_spinlock);

  return count;
}

void init_kprintf_spinlock(void)
{
  spinlock_init(&kprintf_spinlock);
  kprintf("Kprintf spinlock...");
  kok();
}
