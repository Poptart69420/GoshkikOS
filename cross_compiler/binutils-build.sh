export PREFIX="$PWD/build"
export PATH="$HOME/opt/autoconf-2.69/bin:$PATH"

mkdir build/binutils
cd build/binutils
../binutils-with-gold-2.44/configure \
    --target=x86_64-shitos \
    --prefix="$PREFIX" \
    --with-sysroot=../sysroot \
    --disable-werror

make
make install
