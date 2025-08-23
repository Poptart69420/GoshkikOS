#!/bin/bash

set -e

export PROJECT_ROOT="$PWD"

export PREFIX="$PROJECT_ROOT/cross_compiler/build"

export SYS_ROOT="$PROJECT_ROOT/sysroot"

export TARGET=x86_64-shitos

export PATH="$PREFIX/bin:$PATH"

BINUTILS_SRC="$PROJECT_ROOT/cross_compiler/binutils-with-gold-2.44"
GCC_SRC="$PROJECT_ROOT/cross_compiler/gcc"

cd "$PROJECT_ROOT/cross_compiler"

echo "--- Building Binutils ---"
rm -rf build/build-binutils
mkdir -p build/build-binutils
cd build/build-binutils
"$BINUTILS_SRC/configure" \
    --target="$TARGET" \
    --prefix="$PREFIX" \
    --with-sysroot="$SYS_ROOT" \
    --disable-nls \
    --disable-werror
make -j$(nproc)
make install
cd ../../

echo "--- Building GCC ---"
rm -rf build/build-gcc
mkdir -p build/build-gcc
cd build/build-gcc
"$GCC_SRC/configure" \
    --target="$TARGET" \
    --prefix="$PREFIX" \
    --with-sysroot="$SYS_ROOT" \
    --disable-nls \
    --enable-languages=c \
    --without-headers
make -j$(nproc) all-gcc
make install-gcc
cd "$PROJECT_ROOT"
