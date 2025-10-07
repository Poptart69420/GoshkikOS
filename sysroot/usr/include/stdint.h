// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

// TCC don't provide stdint
#ifndef __tinycc__
#ifndef _STDINT_H
#define _STDINT_H
#include <stddef.h>

// signed
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

// unsigned
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

// now least

// signed least
typedef signed char int_least8_t;
typedef signed short int_least16_t;
typedef signed int int_least32_t;
typedef signed long long int_least64_t;

// unsigned least
typedef unsigned char uint_least8_t;
typedef unsigned short uint_least16_t;
typedef unsigned int uint_least32_t;
typedef unsigned long long uint_least64_t;

// now fast

// signed fast
typedef signed char int_fast8_t;
typedef signed int int_fast16_t;
typedef signed int int_fast32_t;
typedef signed long long int_fast64_t;

// unsigned fast
typedef unsigned char uint_fast8_t;
typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;
typedef unsigned long long uint_fast64_t;

// max
typedef int64_t intmax_t;
typedef uint64_t uintmax_t;

// ptr
#ifndef __intptr_t_defined
#define __intptr_t_defined 1
typedef __PTRDIFF_TYPE__ intptr_t;
typedef __SIZE_TYPE__ uintptr_t;
#endif

// integer min limits
#define INT8_MIN -128
#define INT16_MIN -32768
#define INT32_MIN (-2147483647 - 1)
#define INT64_MIN (-9223372036854775807LL - 1)

#define INT_LEAST8_MIN INT8_MIN
#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST64_MIN INT64_MIN

#define INT_FAST8_MIN INT8_MIN
#define INT_FAST16_MIN INT32_MIN
#define INT_FAST32_MIN INT32_MIN
#define INT_FAST64_MIN INT64_MIN

#define INTMAX_MIN INT64_MIN
#define INTPTR_MIN INT64_MIN

// integer max limits
#define INT8_MAX 127
#define INT16_MAX 32767
#define INT32_MAX 2147483647
#define INT64_MAX 9223372036854775807LL

#define INT_LEAST8_MAX INT8_MAX
#define INT_LEAST16_MAX INT16_MAX
#define INT_LEAST32_MAX INT32_MAX
#define INT_LEAST64_MAX INT64_MAX

#define INT_FAST8_MAX INT8_MAX
#define INT_FAST16_MAX INT32_MAX
#define INT_FAST32_MAX INT32_MAX
#define INT_FAST64_MAX INT64_MAX

#define INTMAX_MAX INT64_MAX
#define INTPTR_MAX INT64_MAX

#define UINT8_MAX 255
#define UINT16_MAX (INT16_MAX * 2 - 1)
#define UINT32_MAX (INT32_MAX * 2U - 1)
#define UINT64_MAX (INT64_MAX * 2ULL - 1)

#define UINT_LEAST8_MAX UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

#define UINT_FAST8_MAX UINT8_MAX
#define UINT_FAST16_MAX UINT32_MAX
#define UINT_FAST32_MAX UINT32_MAX
#define UINT_FAST64_MAX UINT64_MAX

#define UINTMAX_MAX UINT64_MAX
#define UINTPTR_MAX UINT64_MAX

#ifndef __SIZE_MAX__
#include <limits.h> //we are goinf to use SSIZE_MAX
#define __SIZE_MAX__ (SSIZE_MAX * 2 + 1)
#endif
#ifndef SIZE_MAX
#define SIZE_MAX __SIZE_MAX__
#endif

// literal macros

#define INT8_C(x) x
#define INT16_C(x) x
#define INT32_C(x) x
#define INT64_C(x) x##LL
#define INTMAX_C(x) INT64_C(x)

#define UINT8_C(x) x
#define UINT16_C(x) x
#define UINT32_C(x) x##U
#define UINT64_C(x) x##ULL
#define UINTMAX_C(x) UINT64_C(x)

#endif
#else
#include_next <stdint.h>
#endif

_END_C_HEADER
