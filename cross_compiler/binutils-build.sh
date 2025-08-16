mkdir -p binutils-gdb/build-binutils
cd binutils-gdb/build-binutils

../configure \
    --target=x86_64-shitos \
    --prefix=$HOME/cross-tools \
    --with-sysroot \
    --disable-nls \
    --disable-werror

make -j$(nproc)
make install
cd ..
cd ..
