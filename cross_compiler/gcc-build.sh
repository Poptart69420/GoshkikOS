mkdir -p gcc/build-gcc
cd gcc/build-gcc

../configure \
    --target=x86_64-elf \
    --prefix=$HOME/cross-tools \
    --disable-nls \
    --enable-languages=c \
    --without-headers \
    --disable-libssp \
    --disable-shared \
    --disable-threads \
    --disable-multilib

make -j$(nproc)
make install
cd ..
mv build-gcc ../../gcc
cd ..
