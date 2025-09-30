#ifndef TARFS_H_
#define TARFS_H_

#include "../../../include/hcf.h"
#include "../../vterm/kerror.h"
#include "../../vterm/kok.h"
#include "../../vterm/vterm.h"
#include "../tmpfs/tmpfs.h"

typedef struct ustar_header_struct_t {
    char name[100];
    char file_mode[8];
    char owner[8];
    char group[8];
    char file_size[12];
    char last_modified[12];
    uint64_t checksum;
    char type;
    char linked_file[100];
    char ustar[6];
    char owner_name[32];
    uint64_t major_number;
    uint64_t minor_number;
    char prefix[155];
} __attribute__((packed)) ustar_header_t;

#define CHECK(o, m)                                                            \
    if (i & o)                                                                 \
    mode |= m

#define USTAR_REGTYPE '0'
#define USTAR_AREGTYPE '\0'
#define USTAR_LNKTYPE '1'
#define USTAR_SYMTYPE '2'
#define USTAR_CHRTYPE '3'
#define USTAR_BLKTYPE '4'
#define USTAR_DIRTYPE '5'
#define USTAR_FIFOTYPE '6'
#define USTAR_CONTTYPE '7'

#define TSUID 04000
#define TSGID 02000
#define TSVTX 01000

#define TUREAD 00400
#define TUWRITE 00200
#define TUEXEC 00100
#define TGREAD 00040
#define TGWRITE 00020
#define TGEXEC 00010
#define TOREAD 00004
#define TOWRITE 00002
#define TOEXEC 00001

void mount_initrd(void);

#endif // TARFS_H_
