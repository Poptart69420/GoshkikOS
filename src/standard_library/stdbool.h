#pragma once

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
  #include <stdbool.h>
#else
  typedef enum {
    __false = 0,
    __true = 1
  } __bool;

  #define bool  __bool
  #define false __false
  #define true  __true
#endif
