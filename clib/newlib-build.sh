mkdir -p newlib/build-newlib
cd newlib/build-newlib

../configure \
    --target=x86_64-shitos \
    --prefix=/opt/cross \
    --disable-nls

make -j$(nproc)
make install
cd ..
mv build-newlib ../../newlib
cd ..
