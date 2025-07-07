ENTRY(kmain)

SECTIONS
{
  . = 0x10000;

  .text : {
    *(.text.entry)
    *(.text*)
  }

  .rodata : {
    *(.rodata*)
  }

  .data : {
    *(.data*)
  }

  .bss : {
    *(.bss*)
    *(COMMON)
  }
}
