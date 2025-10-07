// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef INTTYPES_H
#define INTTYPES_H

#include <stdint.h>

// int8
#define PRId8 "hhd"
#define PRIu8 "hhu"
#define PRIo8 "hho"
#define PRIi8 "hhi"
#define PRIx8 "hhx"
#define PRIX8 "hhX"

#define SCNd8 "d"
#define SCNu8 "u"
#define SCNo8 "o"
#define SCNi8 "i"
#define SCNx8 "x"

// int16
#define PRId16 "hd"
#define PRIu16 "hu"
#define PRIo16 "ho"
#define PRIi16 "hi"
#define PRIx16 "hx"
#define PRIX16 "hX"

#define SCNd16 "d"
#define SCNu16 "u"
#define SCNo16 "o"
#define SCNi16 "i"
#define SCNx16 "x"

// int32
#define PRId32 "d"
#define PRIu32 "u"
#define PRIo32 "o"
#define PRIi32 "i"
#define PRIx32 "x"
#define PRIX32 "X"

#define SCNd32 "d"
#define SCNu32 "u"
#define SCNo32 "o"
#define SCNi32 "i"
#define SCNx32 "x"

// int64
#define PRId64 "lld"
#define PRIu64 "llu"
#define PRIo64 "llo"
#define PRIi64 "lli"
#define PRIx64 "llx"
#define PRIX64 "llX"

#define SCNd64 "lld"
#define SCNu64 "llu"
#define SCNo64 "llo"
#define SCNi64 "lli"
#define SCNx64 "llx"

// least8
#define PRIdLEAST8 PRId8
#define PRIuLEAST8 PRIu8
#define PRIoLEAST8 PRIo8
#define PRIiLEAST8 PRIi8
#define PRIxLEAST8 PRIx8
#define PRIXLEAST8 PRIX8

#define SCNdLEAST8 SCNd8
#define SCNuLEAST8 SCNu8
#define SCNoLEAST8 SCNo8
#define SCNiLEAST8 SCNi8
#define SCNxLEAST8 SCNx8

// least16
#define PRIdLEAST16 PRId16
#define PRIuLEAST16 PRIu16
#define PRIoLEAST16 PRIo16
#define PRIiLEAST16 PRIi16
#define PRIxLEAST16 PRIx16
#define PRIXLEAST16 PRIX16

#define SCNdLEAST16 SCNd16
#define SCNuLEAST16 SCNu16
#define SCNoLEAST16 SCNo16
#define SCNiLEAST16 SCNi16
#define SCNxLEAST16 SCNx16

// least32
#define PRIdLEAST32 PRId32
#define PRIuLEAST32 PRIu32
#define PRIoLEAST32 PRIo32
#define PRIiLEAST32 PRIi32
#define PRIxLEAST32 PRIx32
#define PRIXLEAST32 PRIX32

#define SCNdLEAST32 SCNd32
#define SCNuLEAST32 SCNu32
#define SCNoLEAST32 SCNo32
#define SCNiLEAST32 SCNi32
#define SCNxLEAST32 SCNx32

// least64
#define PRIdLEAST64 PRId64
#define PRIuLEAST64 PRIu64
#define PRIoLEAST64 PRIo64
#define PRIiLEAST64 PRIi64
#define PRIxLEAST64 PRIx64
#define PRIXLEAST64 PRIX64

#define SCNdLEAST64 SCNd64
#define SCNuLEAST64 SCNu64
#define SCNoLEAST64 SCNo64
#define SCNiLEAST64 SCNi64
#define SCNxLEAST64 SCNx64

// fast8
#define PRIdFAST8 PRId8
#define PRIuFAST8 PRIu8
#define PRIoFAST8 PRIo8
#define PRIiFAST8 PRIi8
#define PRIxFAST8 PRIx8
#define PRIXFAST8 PRIX8

#define SCNdFAST8 SCNd8
#define SCNuFAST8 SCNu8
#define SCNoFAST8 SCNo8
#define SCNiFAST8 SCNi8
#define SCNxFAST8 SCNx8

// fast16
#define PRIdFAST16 PRId32
#define PRIuFAST16 PRIu32
#define PRIoFAST16 PRIo32
#define PRIiFAST16 PRIi32
#define PRIxFAST16 PRIx32
#define PRIXFAST16 PRIX32

#define SCNdFAST16 SCNd32
#define SCNuFAST16 SCNu32
#define SCNoFAST16 SCNo32
#define SCNiFAST16 SCNi32
#define SCNxFAST16 SCNx32

// fast32
#define PRIdFAST32 PRId32
#define PRIuFAST32 PRIu32
#define PRIoFAST32 PRIo32
#define PRIiFAST32 PRIi32
#define PRIxFAST32 PRIx32
#define PRIXFAST32 PRIX32

#define SCNdFAST32 SCNd32
#define SCNuFAST32 SCNu32
#define SCNoFAST32 SCNo32
#define SCNiFAST32 SCNi32
#define SCNxFAST32 SCNx32

// fast64
#define PRIdFAST64 PRId64
#define PRIuFAST64 PRIu64
#define PRIoFAST64 PRIo64
#define PRIiFAST64 PRIi64
#define PRIxFAST64 PRIx64
#define PRIXFAST64 PRIX64

#define SCNdFAST64 SCNd64
#define SCNuFAST64 SCNu64
#define SCNoFAST64 SCNo64
#define SCNiFAST64 SCNi64
#define SCNxFAST64 SCNx64

// max
#define PRIdMAX PRId64
#define PRIuMAX PRIu64
#define PRIoMAX PRIo64
#define PRIiMAX PRIi64
#define PRIxMAX PRIx64
#define PRIXMAX PRIX64

#define SCNdMAX SCNd64
#define SCNuMAX SCNu64
#define SCNoMAX SCNo64
#define SCNiMAX SCNi64
#define SCNxMAX SCNx64

// ptr
#define PRIdPTR PRId64
#define PRIuPTR PRIu64
#define PRIoPTR PRIo64
#define PRIiPTR PRIi64
#define PRIxPTR PRIx64
#define PRIXPTR PRIX64

#define SCNdPTR SCNd64
#define SCNuPTR SCNu64
#define SCNoPTR SCNo64
#define SCNiPTR SCNi64
#define SCNxPTR SCNx64

// imaxdiv
typedef struct {
  intmax_t quot;
  intmax_t rem;
} imaxdiv_t;

// intmax operation functions
intmax_t imaxabs(intmax_t j);
imaxdiv_t imaxdiv(intmax_t numer, intmax_t denom);

#endif

_END_C_HEADER
