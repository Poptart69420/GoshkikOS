// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef SYS_TIME_H
#define SYS_TIME_H

#include <sys/type.h>

struct timeval {
  time_t tv_sec;       // seconds
  suseconds_t tv_usec; // microseconds
};

struct timespec {
  time_t tv_sec; // seconds
  long tv_nsec;  // nanoseconds
};

struct timezone {
  int tz_minuteswest;
  int tz_dsttime;
};

typedef long clock_t;
typedef int clockid_t;

#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1

int gettimeofday(struct timeval *tv, struct timezone *tz);

#endif

_END_C_HEADER
