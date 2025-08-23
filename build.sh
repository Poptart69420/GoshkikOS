#!/usr/bin/env sh

export PATH="$PWD/cross_compiler/build/bin:$PATH"
make clean && make TOOLCHAIN=x86_64-shitos
