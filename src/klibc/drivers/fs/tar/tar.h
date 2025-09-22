#ifndef TAR_H_
#define TAR_H_

#include <stdint.h>
#include "../vfs.h"
#include "../../../include/hcf.h"

typedef struct tar_header_struct
{
  char name[100];
  char file_mode[8];
  char owner[8];
  char group[8];
  char file_size[12];
  char last_modif[12];
  uint64_t checksum;
  char type;
  char linked_file[100];
  char magic[6];
  char owner_name[32];
  char group_name[32];
  uint64_t major_number;
  uint64_t minor_number;
  char prefix[155];
}__attribute__((packed)) tar_header;

void mount_initrd(void);

#endif // TAR_H_
