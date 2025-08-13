mkdir -p newlib/build-newlib
cd newlib/build-newlib

CFLAGS="-g -O2" CXXFLAGS="-g -O2" ../configure \
  --target=x86_64-shitos \
  --prefix=$HOME/cross-tools/ \
  --disable-newlib-supplied-syscalls \
  --enable-dependency-tracking

make -j$(nproc)
make install
cd ..
mv build-newlib ../../newlib
cd ..
