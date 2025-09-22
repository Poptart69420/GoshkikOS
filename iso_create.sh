cd build
rm -rf bin
rm -rf obj

mkdir -p bin
mkdir -p obj

cd ..

make

cd bootloader

rm -rf Limine

# Download the latest Limine binary release for the 9.x branch.
git clone https://codeberg.org/Limine/Limine.git --branch=v9.x-binary --depth=1

# Build "limine" utility.
make -C Limine

cd ..

# Create a directory which will be our ISO root.
mkdir -p iso_root

# Copy the relevant files over.
mkdir -p iso_root/boot
cp -v build/bin/goshkikOS iso_root/boot/
cp -v build/boot/initrd.tar iso_root/boot/
mkdir -p iso_root/boot/limine
cp -v bootloader/limine.conf bootloader/Limine/limine-bios.sys bootloader/Limine/limine-bios-cd.bin \
      bootloader/Limine/limine-uefi-cd.bin iso_root/boot/limine/

# Create the EFI boot tree and copy Limine's EFI executables over.
mkdir -p iso_root/EFI/BOOT
cp -v bootloader/Limine/BOOTX64.EFI iso_root/EFI/BOOT/
cp -v bootloader/Limine/BOOTIA32.EFI iso_root/EFI/BOOT/

# Create the bootable ISO.
xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
        -apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso_root -o goshkikOS.iso

# Install Limine stage 1 and 2 for legacy BIOS boot.
bootloader/Limine/limine bios-install goshkikOS.iso
