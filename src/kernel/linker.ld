ENTRY(kmain)

SECTIONS
{
  . = 0x8200;

  .text : ALIGN(0x1000) {
    *(.text.entry)
    *(.text*)
  }

  .rodata : ALIGN(0x1000) {
    *(.rodata*)
  }

  .data : ALIGN(0x1000) {
    *(.data*)
  }

  .bss : ALIGN(0x1000) {
    *(.bss*)
    *(COMMON)
  }
}
