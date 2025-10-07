#include <arch/x86_64/ps2/ps2.h>

uint8_t ps2_read(void)
{
  while ((inb(PS2_COMMAND) & 1) == 0)
  {
    __asm__ volatile("pause");
  }
  return inb(PS2_DATA);
}

void ps2_write(uint16_t port, uint8_t value)
{
  while ((inb(PS2_COMMAND) & 2) != 0)
  {
    __asm__ volatile("pause");
  }
  outb(port, value);
}

uint8_t get_ps2_config(void)
{
  ps2_write(PS2_COMMAND, 0x20);
  return ps2_read();
}

void write_ps2_config(uint8_t value)
{
  ps2_write(PS2_COMMAND, 0x60);
  ps2_write(PS2_DATA, value);
}

uint64_t ps2_entry(void)
{
  ps2_write(PS2_COMMAND, PS2_DISABLE_P1);
  ps2_write(PS2_COMMAND, PS2_DISABLE_P2);

  uint8_t ps2_config = get_ps2_config();

  ps2_config |= (1 << 0) | (1 << 6);

  if ((ps2_config & (1 << 5)) != 0)
  {
    ps2_config |= (1 << 1);
  }

  write_ps2_config(ps2_config);

  ps2_write(PS2_COMMAND, PS2_ENABLE_P1);

  vterm_print("PS/2...");
  kok();

  return 0;
}
