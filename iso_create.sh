cd build
rm -rf bin
rm -rf obj

mkdir bin
mkdir obj

cd ..

make

cd bootloader

# Download the latest Limine binary release for the 9.x branch.
git clone https://github.com/limine-bootloader/limine.git --branch=v9.x-binary --depth=1

# Build "limine" utility.
make -C limine

cd ..

# Create a directory which will be our ISO root.
mkdir -p iso_root

# Copy the relevant files over.
mkdir -p iso_root/boot
cp -v build/bin/shitos iso_root/boot/
mkdir -p iso_root/boot/limine
cp -v bootloader/limine.conf bootloader/limine/limine-bios.sys bootloader/limine/limine-bios-cd.bin \
      bootloader/limine/limine-uefi-cd.bin iso_root/boot/limine/

# Create the EFI boot tree and copy Limine's EFI executables over.
mkdir -p iso_root/EFI/BOOT
cp -v bootloader/limine/BOOTX64.EFI iso_root/EFI/BOOT/
cp -v bootloader/limine/BOOTIA32.EFI iso_root/EFI/BOOT/

# Create the bootable ISO.
xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
        -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso_root -o shitos.iso

# Install Limine stage 1 and 2 for legacy BIOS boot.
bootloader/limine/limine bios-install shitos.iso
