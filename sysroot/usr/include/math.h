//FROM FILE COMES FROM TLIBC (tayoky 2025)
//SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef MATH_H
#define MATH_H

#include <limits.h>
#include <float.h>

typedef float float_t;
typedef double double_t;

//macro
#define fpclassify(x)
#define isfinite(x) __builtin_isfinite(x)
#define isinf(x) __builtin_isinf_sign(x)
#define isnan(x) __builtin_isnan(x)
#define isnormal(x)

//builtin are only faster on GCC
#if defined(__GNUC__) && !defined(__clang__)
#define signbit(x) __builtin_signbit(x)
#define isgreater(x,y) __builtin_isgreater(x,y)
#define isgreaterequal(x,y) __builtin_isgreaterequal(x,y)
#define isless(x,y) __builtin_isless(x,y)
#define islessequal(x,y) __builtin_islessequal(x,y)
#define islessgreater(x,y) __builtin_islessgreater(x,y)
#else
#define signbit(x) (x < 0)
#define func(x,y,op) (isunordered(x,y) ? NAN : (op))
#define isgreater(x,y) func(x,y,x > y)
#define isgreaterequal(x,y) func(x,y,x >= y)
#define isless(x,y) func(x,y,x < y)
#define islessequal(x,y) func(x,y,x <= y)
#define islessgreater(x,y) func(x,y,x != y)
#undef func
#endif

#define isunordered(x,y) (isnan(x) || isnan(y))

#define ifunc(name) int name(int x);\
	long l##name(long x);\
	long long ll##name(long long x);
#define ffunc(name) float name##f(float x);\
	double name(double x);\
	long double name##l(long double x);
#define ffunc2(name) float name##f(float x,float y);\
	double name(double x,double y);\
	long double name##l(long double x,long double y);

//abs
ffunc(fabs)

ffunc(cos)
ffunc(sin)
ffunc(tan)

ffunc2(fmin)
ffunc2(fmax)

ffunc(floor)
ffunc(ceil)
ffunc(round)

ffunc(sqrt)

ffunc(log2)

float ldexpf(float a,int exp);
double ldexp(double a,int exp);
long double ldexpl(long double a,int exp);

float powf(float x,float y);

#undef ifunc
#undef ffunc
#undef ffunc2

//constant

#define M_PI 3.1415926535f
#define M_PI_2 (M_PI * 0.5f)
#define M_PI_4 (M_PI * 0.25f)
#define M_1_PI (1.0f / M_PI)
#define M_2_PI (2.0f / M_PI)
#ifndef NAN
#define NAN __builtin_nanf("")
#endif
#ifndef INFINITY
#define INFINITY __builtin_inff()
#endif
#define HUGE_VALF __builtin_huge_valf()
#define HUGE_VAL  __builtin_huge_val()
#define HUGE_VALL __builtin_huge_vall()

#endif

_END_C_HEADER
