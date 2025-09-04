#!/usr/bin/env sh

mkdir -p build
export PATH="$PWD/cross_compiler/build/bin:$PATH"

make clean && make TOOLCHAIN=x86_64-shitos | GREP_COLORS='mt=01;31' grep --color=always -e '^Makefile:[0-9]\+:.*  Stop\.$' -e '^' \
     | GREP_COLORS='mt=01;33' grep --color=always -e '^Makefile:[0-9]\+:.*' -e '^'

./iso_create.sh
