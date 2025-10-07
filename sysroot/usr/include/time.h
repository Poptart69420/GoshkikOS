// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef TIME_H
#define TIME_H

#include <locale.h>
#include <sys/time.h>

struct tm
{
  int tm_sec;   // Seconds. [0-60] 1 leap second
  int tm_min;   // Minutes. [0-59]
  int tm_hour;  // Hours.   [0-23]
  int tm_mday;  // Day.     [1-31]
  int tm_mon;   // Month.   [0-11]
  int tm_year;  // Year - 1900.
  int tm_wday;  // Day of week. [0-6]
  int tm_yday;  // Days in year.[0-365]
  int tm_isdst; // DST.     [-1/0/1]

  long int tm_gmtoff;  // Seconds east of UTC.
  const char *tm_zone; // STimezone abbreviation.SS
};

time_t time(time_t *tloc);
struct tm *gmtime(const time_t *clock);
struct tm *gmtime_r(const time_t *clock, struct tm *tm);
struct tm *localtime(const time_t *clock);
struct tm *localtime_r(const time_t *clock, struct tm *tm);
char *asctime(const struct tm *timeptr);
char *asctime_r(const struct tm *timeptr, char *buf);
char *ctime(const time_t *clock);
char *ctime_r(const time_t *clock, char *buf);
time_t mktime(struct tm *);
int clock_gettime(clockid_t clock_id, struct timespec *tp);
int clock_settime(clockid_t clock_id, struct timespec *tp);
int nanosleep(const struct timespec *, struct timespec *);
void tzset(void);
size_t strftime(char *, size_t, const char *, const struct tm *);
size_t strftime_l(char *buf, size_t size, const char *fmt, const struct tm *tm,
                  locale_t locale);

extern char *tzname[2];
extern long timezone;
extern int daylight;

#endif

_END_C_HEADER
