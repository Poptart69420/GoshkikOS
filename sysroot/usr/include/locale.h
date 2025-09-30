//FROM FILE COMES FROM TLIBC (tayoky 2025)
//SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef _LOCALE_H
#define _LOCALE_H

#define LC_COLLATE  0
#define LC_CTYPE    1
#define LC_MONETARY 2
#define LC_MESSAGES 3
#define LC_NUMERIC  4
#define LC_TIME     5
#define LC_ALL      6

#ifndef NULL
#define NULL (void*)0
#endif

struct lconv {
	char currency_symbol[8];
	char decimal_point[8];
	char frac_digits;
	char grouping[8];
	char int_curr_symbol[8];
	char int_frac_digits;
	char mon_decimal_point[8];
	char mon_grouping[8];
	char mon_thousands_sep[8];
	char negative_sign[8];
	char n_cs_precedes;
	char n_sep_by_space;
	char n_sign_posn;
	char positive_sign[8];
	char p_cs_precedes;
	char p_sep_by_space;
	char p_sign_posn;
	char thousands_sep[8];
};

struct locale_time {
	char day[7][16];
	char abday[7][4];
	char mon[12][16];
	char abmon[12][4];
	char am[8];
	char pm[8];
	char d_t_fmt[32];
	char d_fmt[16];
	char t_fmt[16];
	char t_fmt_ampm[16];
};

struct locale_messages {
	char yesexpr[8];
	char noexpr[8];
};

struct __locale {
	struct lconv lconv;
	int mb_type;
	int ctype[256];
	int tolower[256];
	int toupper[256];
	struct locale_time time;
	struct locale_messages messages;
}; 
typedef struct __locale *locale_t;

#define LOCAL_MB_UTF8   0
#define LOCAL_MB_UTF16  1
#define LOCAL_MB_UTF32  2
#define LOCAL_UPPER     0x01
#define LOCAL_LOWWER    0x02
#define LOCAL_DIGIT     0x04
#define LOCAL_ALPHA     0x08
#define LOCAL_PUNCT     0x10
#define LOCAL_

char *setlocale(int category,const char *locale);
struct lconv *localeconv(void);
locale_t uselocale(locale_t locale);
#endif

_END_C_HEADER
