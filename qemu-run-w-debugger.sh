#!/bin/bash

qemu-system-x86_64 -cdrom goshkikOS.iso \
    -no-reboot -no-shutdown \
    -d int,cpu,exec,guest_errors \
    -monitor stdio
