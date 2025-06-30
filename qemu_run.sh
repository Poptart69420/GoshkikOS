qemu-system-x86_64 \
-drive file=build/main_floppy.img,format=raw \
  -no-reboot -no-shutdown \
  -d int,cpu,exec,guest_errors
