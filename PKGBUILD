# Maintainer: Your Name <your@email.com>
pkgname=wordle
pkgver=1.0
pkgrel=1
pkgdesc="Terminal Wordle clone with ANSI colors"
arch=('x86_64')
license=('Unlicense')
depends=('gcc-libs')
makedepends=('gcc')

build() {
    g++ -std=c++17 -O2 -o "$startdir/$pkgname" "$startdir/src/main.cpp"
}

package() {
    install -Dm755 "$startdir/$pkgname" "$pkgdir/usr/bin/$pkgname"
}
