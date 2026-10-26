# violet was taken
pkgname=violet-ssg
pkgver=$VERSION
pkgrel=1
pkgdesc="AI slop-free static site generator"
# We _should_ be supporting any, but violet is only tested on x86_64
arch=('any')
depends=('glibc')
makedepends=('gcc' 'cmake' 'make')
source=("https://codeberg.org/LunarWatcher/violet/archive/$pkgver.tar.gz")
sha256sums=("$CHECKSUM")
build() {
    cd "$srcdir/violet"
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DVIOLET_BUILD_VERSION="$VERSION"
}
check() {
    cd "$srcdir/violet/build"
    make test -j $(nproc)
}
package() {
    cd "$srcdir/violet/build"
    DESTDIR="$pkgdir" make install -j $(nproc)
}
