#include "pparse.h"

void path_parse(const char *raw, path_t *out)
{
  if (!raw || !out) return;

  size_t part_index = 0;
  size_t i = 0;
  size_t length = strlen(raw);
  out->count = 0;

  while (i < length && part_index < MAX_PATH_PARTS) {
    while (i < length && raw[i] == '/') ++i;
      if (i >= length) break;

      size_t j = 0;
      while (i < length && raw[i] != '/' && j < 255) {
        out->parts[part_index][j++] = raw[i++];
      }

      if (j == 0) continue;

      out->parts[part_index][j] = '\0';
      out->count++;
      part_index++;
  }
}
