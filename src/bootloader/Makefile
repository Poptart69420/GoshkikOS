.PHONY: all

ASM?=nasm
BUILD_DIR?=build

all: $(BUILD_DIR)/STAGE1.BIN $(BUILD_DIR)/STAGE2.BIN

$(BUILD_DIR)/STAGE1.BIN: stage_1/stage1.asm | $(BUILD_DIR)
	$(ASM) -f bin stage_1/stage1.asm -o $@


$(BUILD_DIR)/STAGE2.BIN: stage_2/stage2.asm | $(BUILD_DIR)
	$(ASM) -f bin stage_2/stage2.asm -o $@

$(BUILD_DIR):
	mkdir -p $@
