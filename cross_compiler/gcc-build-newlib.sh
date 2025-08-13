mkdir -p gcc/build
cd gcc/build

../configure \
    --target=x86_64-shitos \
    --prefix=$HOME/cross-tools \
    --disable-nls \
    --enable-languages=c \
    --with-newlib \
    --disable-libssp \
    --disable-multilib

make -j$(nproc)
make install
cd ..
mv build ../../gcc
cd ..
