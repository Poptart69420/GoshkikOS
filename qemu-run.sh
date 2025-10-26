#!/bin/bash

qemu-system-x86_64 -cdrom goshkikOS.iso -m 2G -enable-kvm -serial stdio -no-reboot
