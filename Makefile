ASM=nasm

SRC_DIR     = src
BUILD_DIR   = build
X86_64_DIR  = $(BUILD_DIR)/x86_64
ROOT_DIR    = $(BUILD_DIR)/initramfs

BOOTLD_DIR  = $(SRC_DIR)/bootloader
KERNEL_DIR  = $(SRC_DIR)/kernel

STAGE1_BIN  = $(BUILD_DIR)/STAGE1.BIN
STAGE2_BIN  = $(BUILD_DIR)/STAGE2.BIN
KERNEL_BIN  = $(BUILD_DIR)/KERNEL.BIN

INITRAMFS_CPIO = $(BUILD_DIR)/initramfs.cpio
INITRAMFS_OBJ  = $(BUILD_DIR)/initramfs.o

FLOPPY_IMG  = $(BUILD_DIR)/main_floppy.img

SECTOR_SIZE = 512

.PHONY: all clean

all: floppy_image print-sizes

floppy_image: bootloader kernel
	@echo "[+] - Creating blank floppy image"
	dd if=/dev/zero of=$(FLOPPY_IMG) bs=512 count=2880
	@echo "[+] - Copying STAGE1.BIN to boot sector (LBA 0)"
	dd if=$(STAGE1_BIN) of=$(FLOPPY_IMG) bs=512 count=1 conv=notrunc
	@echo "[+] - Copying STAGE2.BIN to LBA 1+"
	dd if=$(STAGE2_BIN) of=$(FLOPPY_IMG) bs=512 seek=1 conv=notrunc
	@echo "[+] - Copying KERNEL.BIN to LBA 3+"
	dd if=$(KERNEL_BIN) of=$(FLOPPY_IMG) bs=512 seek=3 conv=notrunc

bootloader: $(STAGE1_BIN) $(STAGE2_BIN)

bootloader_build: | $(BUILD_DIR)
	$(MAKE) -C $(BOOTLD_DIR) BUILD_DIR=$(abspath $(BUILD_DIR))

$(STAGE1_BIN): bootloader_build
$(STAGE2_BIN): bootloader_build

kernel: | $(BUILD_DIR) $(X86_64_DIR) #$(INITRAMFS_OBJ)
	$(MAKE) -C $(KERNEL_DIR) BUILD_DIR=$(abspath $(BUILD_DIR))

print-sizes:
	@echo "Stage 1 size in bytes : $$(stat -c %s $(STAGE1_BIN))"
	@echo "Stage 1 sector count  : $$((($$(stat -c %s $(STAGE1_BIN)) + $(SECTOR_SIZE) - 1) / $(SECTOR_SIZE)))"
	@echo "Stage 2 size in bytes : $$(stat -c %s $(STAGE2_BIN))"
	@echo "Stage 2 sector count  : $$((($$(stat -c %s $(STAGE2_BIN)) + $(SECTOR_SIZE) - 1) / $(SECTOR_SIZE)))"
	@echo "Kernel size in bytes  : $$(stat -c %s $(KERNEL_BIN))"
	@echo "Kernel sector count   : $$((($$(stat -c %s $(KERNEL_BIN)) + $(SECTOR_SIZE) - 1) / $(SECTOR_SIZE)))"

$(BUILD_DIR):
	mkdir -p $@

$(X86_64_DIR):
	mkdir -p $@

#$(ROOT_DIR):
#	mkdir -p $(ROOT_DIR)/{bin,etc,proc,sys,usr/{bin,lib,include},lib,dev,tmp,var,home}
#	cp -r src/standard_library/headers/* $(ROOT_DIR)/usr/include

#$(INITRAMFS_CPIO): $(ROOT_DIR)
#	@echo "[+] - Creating initramfs archive"
#	cd $(ROOT_DIR) && find . | cpio -o -H newc > ../initramfs.cpio

#$(INITRAMFS_OBJ): $(INITRAMFS_CPIO)
#	@echo "[+] - Embedding initramfs.cpio into object file"
#	ld -r -b binary -o $@ $<

clean:
	rm -rf $(BUILD_DIR)/*
