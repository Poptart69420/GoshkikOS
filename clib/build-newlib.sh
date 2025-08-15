mkdir -p newlib/build-newlib
cd newlib/build-newlib

../configure \
    --target=x86_64-shitos \
    --prefix=$SHITOS_PATH/cross-tools \
    --disable-multilib

make all
make DESTDIR=${SYSROOT} install

mkdir -p x86_64-shitos/newlib/libc/sys/shitos/.deps

make all
make DESTDIR=${SYSROOT} install

cd ..
mv build-newlib ../../newlib
cd ..
