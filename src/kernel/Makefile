.PHONY: all

CC           ?= gcc
LD 			 ?= ld
OBJCOPY ?= objcopy

BUILD_DIR    := $(abspath $(BUILD_DIR))
KERNEL_C     := kernel.c
LINKER       := linker.ld

KERNEL_OBJ   := $(BUILD_DIR)/kernel.o
KERNEL_ELF   := $(BUILD_DIR)/kernel.elf
KERNEL_BIN   := $(BUILD_DIR)/KERNEL.BIN

CFLAGS       := -nostdlib -nostdinc -I $(LIB_DIR) -m64 -ffreestanding -O2 -Wall -Wextra -g -O0
LDFLAGS      := -m elf_x86_64 -T $(LINKER) -g -O0

all: $(KERNEL_BIN)

# Compile kernel C file
$(KERNEL_OBJ): $(KERNEL_C) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link everything into a kernel ELF file
$(KERNEL_ELF): $(KERNEL_OBJ)
	$(LD) $(LDFLAGS) $^ -o $@

# Convert ELF to flat binary
$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@
