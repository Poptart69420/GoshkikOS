mkdir -p gcc/build-gcc
cd gcc/build-gcc

../configure \
    --target=x86_64-shitos \
    --prefix=$HOME/cross-tools \
    --disable-nls \
    --enable-languages=c \
    --without-headers \
    --disable-shared \
    --disable-threads \
    --disable-multilib \
    --disable-libssp \
    --disable-libgomp \
    --disable-libatomic \
    --disable-libstdcxx-pch \

make all-gcc -j$(nproc)
make all-target-libgcc -j$(nproc)
make install-gcc
make install-target-libgcc
cd ..
mv build-gcc ../../gcc
cd ..
