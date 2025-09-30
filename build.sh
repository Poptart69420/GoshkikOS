#!/usr/bin/env sh

set -e

source ./add-to-path.sh

rm -rf build
mkdir -p build
export PATH="$PWD/cross_compiler/build/bin:$PATH"

make clean && make TOOLCHAIN=x86_64-goshkikos

unset CC

./iso_create.sh
