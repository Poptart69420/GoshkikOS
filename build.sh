#!/usr/bin/env sh

rm -rf build
mkdir -p build
export PATH="$PWD/cross_compiler/build/bin:$PATH"

make clean && make TOOLCHAIN=x86_64-elf

./iso_create.sh
