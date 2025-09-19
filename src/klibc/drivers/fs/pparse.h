#ifndef PPARSE_H_
#define PPARSE_H_

#include "stdint.h"
#include "stddef.h"
#include "../../mem/string.h"

#define MAX_PATH_PARTS 32

typedef struct
{
  char parts[MAX_PATH_PARTS][256];
  size_t count;
} path_t;

void path_parse(const char* raw, path_t* out);

#endif // PPARSE_H_
