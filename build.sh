#!/usr/bin/env sh

export PATH="$PWD/cross_compiler/build/bin:$PATH"
x86_64-shitos-gcc -c src/crt0.s -o build/crt0.o -g
x86_64-shitos-gcc -c src/crti.s -o build/crti.o -g
x86_64-shitos-gcc -c src/crtn.s -o build/crtn.o -g
make clean && make TOOLCHAIN=x86_64-shitos
./iso_create.sh
