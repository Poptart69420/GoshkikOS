.PHONY: all clean

ASM       ?= nasm
CC        ?= gcc
LD        ?= ld
OBJCOPY   ?= objcopy

BUILD_DIR := $(abspath build)
LIB_DIR   := ../standard_library
INC_DIR   := ./x86_64
OBJ_DIR   := $(BUILD_DIR)/x86_64

KERNEL_C      := kernel.c
LINKER        := linker.ld
KERNEL_OBJ    := $(BUILD_DIR)/kernel.o
KERNEL_ELF    := $(BUILD_DIR)/kernel.elf
KERNEL_BIN    := $(BUILD_DIR)/KERNEL.BIN

C_SOURCES     := $(wildcard $(INC_DIR)/*.c)
ASM_SOURCES   := $(wildcard $(INC_DIR)/*.asm)

C_OBJS        := $(patsubst $(INC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
ASM_OBJS      := $(patsubst $(INC_DIR)/%.asm,$(OBJ_DIR)/%.o,$(ASM_SOURCES))

CFLAGS    := -nostdlib -nostdinc -I $(LIB_DIR) -m64 -ffreestanding -O2 -Wall -Wextra -g -O0
LDFLAGS   := -m elf_x86_64 -T $(LINKER) -g -O0

all: $(KERNEL_BIN)

$(BUILD_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

$(KERNEL_OBJ): $(KERNEL_C) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile included C files
$(OBJ_DIR)/%.o: $(INC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile included ASM files
$(OBJ_DIR)/%.o: $(INC_DIR)/%.asm | $(OBJ_DIR)
	$(ASM) -f elf64 $< -o $@

# Link everything into ELF
$(KERNEL_ELF): $(KERNEL_OBJ) $(C_OBJS) $(ASM_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

# Convert ELF to flat binary
$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR)
