#ifndef TAR_H_
#define TAR_H_

#include <stdint.h>
#include "../vfs.h"
#include "../../../include/hcf.h"
#include "../../../../kernel.h"

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

#define TAR_REGTYPE  '0'        // Regular file
#define TAR_AREGTYPE '\0'       // Regular file
#define TAR_LNKTYPE  '1'        // Hard link
#define TAR_SYMTYPE  '2'        // Symbolic link
#define TAR_CHRTYPE  '3'        // Character special
#define TAR_BLKTYPE  '4'        // Block special
#define TAR_DIRTYPE  '5'        // Directory
#define TAR_FIFOTYPE '6'        // Named pipe
#define TAR_CONTTYPE '7'        // Contiguous file

void mount_initrd(void);

#endif // TAR_H_
