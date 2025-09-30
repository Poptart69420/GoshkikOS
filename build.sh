#!/usr/bin/env sh

set -e
rm -rf build
mkdir -p build
export PATH="$PWD/cross_compiler/build/bin:$PATH"

make clean && make TOOLCHAIN=x86_64-goshkikos

./iso_create.sh
