// FROM FILE COMES FROM TLIBC (tayoky 2025)
// SEE https://github.com/tayoky/tlibc FOR LASTED VERSION

#include <sys/cdefs.h>

_BEGIN_C_HEADER

#ifndef ELF_H
#define ELF_H

#include <stdint.h>

// 32-bit ELF base types
typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

// 64-bit ELF base types
typedef uint64_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef int16_t Elf64_SHalf;
typedef uint64_t Elf64_Off;
typedef int32_t Elf64_Sword;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Xword;
typedef int64_t Elf64_Sxword;

#define SHN_UNDEF 0
#define SHN_LORESERVE 0xff00
#define SHN_LOPROC 0xff00
#define SHN_HIPROC 0xff1f
#define SHN_ABS 0xfff1
#define SHN_COMMON 0xfff2
#define SHN_HIRESERVE 0xffff

// headers

// header const

#define ET_NONE 0

// e_type
#define ET_REL 1         // Relocatable file
#define ET_EXEC 2        // Executable file
#define ET_DYN 3         // Shared object file
#define ET_CORE 4        // Core file
#define ET_LOPROC 0xff00 // Processor-specific
#define ET_HIPROC 0xffff // Processor-specific

// e_machine
#define EM_M32 1          // AT&T WE 32100
#define EM_SPARC 2        // SPARC
#define EM_386 3          // Intel Architecture
#define EM_68K 4          // Motorola 68000
#define EM_88K 5          // Motorola 88000
#define EM_860 7          // Intel 80860
#define EM_MIPS 8         // MIPS RS3000 Big-Endian
#define EM_MIPS_RS4_BE 10 // MIPS RS4000 Big-Endian
#define EM_X86_64 62
#define EM_AARCH64 183

// e_version
#define EV_CURRENT 1 // Current version

// e_ident
#define EI_MAG0 0    // File identification
#define EI_MAG1 1    // File identification
#define EI_MAG2 2    // File identification
#define EI_MAG3 3    // File identification
#define EI_CLASS 4   // File class
#define EI_DATA 5    // Data encoding
#define EI_VERSION 6 // File version
#define EI_PAD 7     // Start of padding bytes
#define EI_NIDENT 16 // Size of e_ident[]

// ELF magic
#define ELFMAG0 0x7f     // e_ident[EI_MAG0]
#define ELFMAG1 'E'      // e_ident[EI_MAG1]
#define ELFMAG2 'L'      // e_ident[EI_MAG2]
#define ELFMAG3 'F'      // e_ident[EI_MAG3]
#define ELFMAG "\177ELF" // ELF magic
#define SELFMAG 4

// EI_CLASS
#define ELFCLASSNONE 0 // Invalid class
#define ELFCLASS32 1   // 32-bit objects
#define ELFCLASS64 2   // 64-bit objects

// EI_DATA
#define ELFDATANONE 0 // Invalid data encoding
#define ELFDATA2LSB 1 // Little-endian
#define ELFDATA2MSB 2 // Big-endian

// 32-bit ELF header
typedef struct
{
  unsigned char e_ident[EI_NIDENT];
  Elf32_Half e_type;
  Elf32_Half e_machine;
  Elf32_Word e_version;
  Elf32_Addr e_entry;
  Elf32_Off e_phoff;
  Elf32_Off e_shoff;
  Elf32_Word e_flags;
  Elf32_Half e_ehsize;
  Elf32_Half e_phentsize;
  Elf32_Half e_phnum;
  Elf32_Half e_shentsize;
  Elf32_Half e_shnum;
  Elf32_Half e_shstrndx;
} Elf32_Ehdr;

// 64-bit ELF header
typedef struct
{
  unsigned char e_ident[EI_NIDENT];
  Elf64_Half e_type;
  Elf64_Half e_machine;
  Elf64_Word e_version;
  Elf64_Addr e_entry;
  Elf64_Off e_phoff;
  Elf64_Off e_shoff;
  Elf64_Word e_flags;
  Elf64_Half e_ehsize;
  Elf64_Half e_phentsize;
  Elf64_Half e_phnum;
  Elf64_Half e_shentsize;
  Elf64_Half e_shnum;
  Elf64_Half e_shstrndx;
} Elf64_Ehdr;

// section header

// 32-bit ELF section header
typedef struct
{
  Elf32_Word sh_name;
  Elf32_Word sh_type;
  Elf32_Word sh_flags;
  Elf32_Addr sh_addr;
  Elf32_Off sh_offset;
  Elf32_Word sh_size;
  Elf32_Word sh_link;
  Elf32_Word sh_info;
  Elf32_Word sh_addralign;
  Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct
{
  Elf64_Word sh_name;
  Elf64_Word sh_type;
  Elf64_Xword sh_flags;
  Elf64_Addr sh_addr;
  Elf64_Off sh_offset;
  Elf64_Xword sh_size;
  Elf64_Word sh_link;
  Elf64_Word sh_info;
  Elf64_Xword sh_addralign;
  Elf64_Xword sh_entsize;
} Elf64_Shdr;

// sh_type / section types
#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11
#define SHT_LOPROC 0x70000000
#define SHT_HIPROC 0x7fffffff
#define SHT_LOUSER 0x80000000
#define SHT_HIUSER 0xffffffff

// sh_flags/sections flags
#define SHF_WRITE 0x01
#define SHF_ALLOC 0x02
#define SHF_EXECINSTR 0x04
#define SHF_MASKPROC 0xf0000000

// symbols

// 32-bit ELF symbol
typedef struct
{
  Elf32_Word st_name;
  Elf32_Addr st_value;
  Elf32_Word st_size;
  unsigned char st_info;
  unsigned char st_other;
  Elf32_Half st_shndx;
} Elf32_Sym;

// 64-bit ELF symbol
typedef struct
{
  Elf64_Word st_name;
  unsigned char st_info;
  unsigned char st_other;
  Elf64_Half st_shndx;
  Elf64_Addr st_value;
  Elf64_Xword st_size;
} Elf64_Sym;

#define ELF32_ST_BIND(i) ((i) >> 4)
#define ELF32_ST_TYPE(i) ((i) & 0xf)
#define ELF32_ST_INFO(b, t) (((b) << 4) + ((t) & 0xf))

#define ELF64_ST_BIND(i) ((i) >> 4)
#define ELF64_ST_TYPE(i) ((i) & 0xf)
#define ELF64_ST_INFO(b, t) (((b) << 4) + ((t) & 0xf))

// ELFXX_ST_BIND
#define STB_LOCAL 0
#define STB_GLOBAL 1
#define STB_WEAK 2
#define STB_LOPROC 13
#define STB_HIPROC 15

// ELFXX_ST_TYPE
#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3
#define STT_FILE 4
#define STT_LOPROC 13
#define STT_HIPROC 15

// relocation

// 32-bit ELF relocation
typedef struct
{
  Elf32_Addr r_offset;
  Elf32_Word r_info;
} Elf32_Rel;

// 32-bit ELF relocation with addend
typedef struct
{
  Elf32_Addr r_offset;
  Elf32_Word r_info;
  Elf32_Sword r_addend;
} Elf32_Rela;

// 64-bit relocation
typedef struct
{
  Elf64_Addr r_offset; // Address of reference
  Elf64_Xword r_info;  // Symbol index and type of relocation
} Elf64_Rel;

// 64-bit relocation with addend
typedef struct
{
  Elf64_Addr r_offset;   // Address of reference
  Elf64_Xword r_info;    // Symbol index and type of relocation
  Elf64_Sxword r_addend; // Constant part of expression
} Elf64_Rela;

#define ELF32_R_SYM(i) ((i) >> 8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))
#define ELF32_R_INFO(s, t) (((s) << 8) + (unsigned char)(t))

#define ELF64_R_SYM(i) ((i) >> 32)
#define ELF64_R_TYPE(i) ((i) & 0xffffffffL)
#define ELF64_R_INFO(s, t) (((s) << 32) + ((t) & 0xffffffffL))

// relocations types

// x86_64
#define R_X86_64_NONE 0      /* No reloc */
#define R_X86_64_64 1        /* Direct 64 bit  */
#define R_X86_64_PC32 2      /* PC relative 32 bit signed */
#define R_X86_64_GOT32 3     /* 32 bit GOT entry */
#define R_X86_64_PLT32 4     /* 32 bit PLT address */
#define R_X86_64_COPY 5      /* Copy symbol at runtime */
#define R_X86_64_GLOB_DAT 6  /* Create GOT entry */
#define R_X86_64_JUMP_SLOT 7 /* Create PLT entry */
#define R_X86_64_RELATIVE 8  /* Adjust by program base */
#define R_X86_64_GOTPCREL 9  /* 32 bit signed PC relative offset to GOT */
#define R_X86_64_32 10       /* Direct 32 bit zero extended */
#define R_X86_64_32S 11      /* Direct 32 bit sign extended */
#define R_X86_64_16 12       /* Direct 16 bit zero extended */
#define R_X86_64_PC16 13     /* 16 bit sign extended pc relative */
#define R_X86_64_8 14        /* Direct 8 bit sign extended  */
#define R_X86_64_PC8 15      /* 8 bit sign extended pc relative */
#define R_X86_64_DTPMOD64 16 /* ID of module containing symbol */
#define R_X86_64_DTPOFF64 17 /* Offset in module's TLS block */
#define R_X86_64_TPOFF64 18  /* Offset in initial TLS block */
#define R_X86_64_TLSGD \
  19 /* 32 bit signed PC relative offset to two GOT entries for GD symbol */
#define R_X86_64_TLSLD \
  20                         /* 32 bit signed PC relative offset to two GOT entries for LD symbol */
#define R_X86_64_DTPOFF32 21 /* Offset in TLS block */
#define R_X86_64_GOTTPOFF \
  22                           /* 32 bit signed PC relative offset to GOT entry for IE symbol */
#define R_X86_64_TPOFF32 23    /* Offset in initial TLS block */
#define R_X86_64_PC64 24       /* PC relative 64 bit */
#define R_X86_64_GOTOFF64 25   /* 64 bit offset to GOT */
#define R_X86_64_GOTPC32 26    /* 32 bit signed pc relative offset to GOT */
#define R_X86_64_GOT64 27      /* 64-bit GOT entry offset */
#define R_X86_64_GOTPCREL64 28 /* 64-bit PC relative offset to GOT entry */
#define R_X86_64_GOTPC64 29    /* 64-bit PC relative offset to GOT */

// program header

// 32-bit ELF program header
typedef struct
{
  Elf32_Word p_type;
  Elf32_Off p_offset;
  Elf32_Addr p_vaddr;
  Elf32_Addr p_paddr;
  Elf32_Word p_filesz;
  Elf32_Word p_memsz;
  Elf32_Word p_flags;
  Elf32_Word p_align;
} Elf32_Phdr;

// 64-bit ELF program header
typedef struct
{
  Elf64_Word p_type;
  Elf64_Word p_flags;
  Elf64_Off p_offset;
  Elf64_Addr p_vaddr;
  Elf64_Addr p_paddr;
  Elf64_Xword p_filesz;
  Elf64_Xword p_memsz;
  Elf64_Xword p_align;
} Elf64_Phdr;

// p_type / program header type
#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7fffffff

// p_flags
#define PF_X 0x1               // Execute
#define PF_W 0x2               // Write
#define PF_R 0x4               // Read
#define PF_MASKPROC 0xf0000000 // Unspecified

#endif

_END_C_HEADER
