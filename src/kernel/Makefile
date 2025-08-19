.PHONY: all clean

TARGET = x86_64-shitos
PREFIX = ../../cross_compiler/build/

SYS_ROOT = ../../sysroot

ASM       ?= nasm
CC        ?= $(PREFIX)/bin/$(TARGET)-gcc
LD        ?= $(PREFIX)/bin/$(TARGET)-ld
OBJCOPY   ?= $(PREFIX)/bin/$(TARGET)-objcopy

BUILD_DIR := $(abspath build)
INC_DIR   := ./x86_64

OBJ_DIR   := $(BUILD_DIR)/x86_64
INT_DIR   := $(INC_DIR)/cpu

KERNEL_C      := kernel.c
LINKER        := linker.ld
KERNEL_OBJ    := $(BUILD_DIR)/kernel.o
KERNEL_ELF    := $(BUILD_DIR)/kernel.elf
KERNEL_BIN    := $(BUILD_DIR)/KERNEL.BIN

C_SOURCES     := $(wildcard $(INC_DIR)/*.c)
ASM_SOURCES   := $(wildcard $(INC_DIR)/*.asm)

C_OBJS        := $(patsubst $(INC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
ASM_OBJS      := $(patsubst $(INC_DIR)/%.asm,$(OBJ_DIR)/%.o,$(ASM_SOURCES))

C_INT_SRCS    := $(wildcard $(INT_DIR)/*.c)
ASM_INT_SRCS  := $(wildcard $(INT_DIR)/*.asm)

C_INT_OBJS    := $(patsubst $(INT_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_INT_SRCS))
ASM_INT_OBJS  := $(patsubst $(INT_DIR)/%.asm,$(OBJ_DIR)/%.o,$(ASM_INT_SRCS))

CFLAGS        := -nostdlib -nostdinc -m64 -ffreestanding -Wall -Wextra -g -O0 -isysroot $(SYS_ROOT) -I$(INC_DIR) -I$(SYS_ROOT)/usr/x86_64-shitos/include
LDFLAGS       := -m elf_x86_64 -T $(LINKER) -g -O0 -L$(SYS_ROOT)/usr/x86_64-shitos/lib -L$(SYS_ROOT)/usr/include -lc

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

# Compile C CPU interrupt files
$(OBJ_DIR)/%.o: $(INT_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile ASM CPU interrupt files
$(OBJ_DIR)/%.o: $(INT_DIR)/%.asm | $(OBJ_DIR)
	$(ASM) -f elf64 $< -o $@

# Link everything into ELF
$(KERNEL_ELF): $(KERNEL_OBJ) $(C_OBJS) $(ASM_OBJS) $(C_INT_OBJS) $(ASM_INT_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@ -L$(PREFIX)/$(TARGET)/lib -lc

# Convert ELF to flat binary
$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

clean:
	rm -rf $(BUILD_DIR)
