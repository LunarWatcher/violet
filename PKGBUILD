# violet was taken
pkgname=violet-ssg
pkgver=$VERSION
pkgrel=$RELEASE
url=https://lunarwatcher.codeberg.page/violet/
pkgdesc="AI slop-free static site generator"
arch=('x86_64' 'i686' 'armv7h' 'aarch64')
license=('MIT')
depends=('glibc')
makedepends=('gcc' 'cmake' 'make' 'git')
source=("$pkgname::git+https://codeberg.org/LunarWatcher/violet.git#tag=$VERSION")
sha256sums=('SKIP')
build() {
    cd "$srcdir/$pkgname"
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
}
check() {
    cd "$srcdir/$pkgname/build"
    make test -j $(nproc)
}
package() {
    cd "$srcdir/$pkgname/build"
    DESTDIR="$pkgdir" make install -j $(nproc)
}
