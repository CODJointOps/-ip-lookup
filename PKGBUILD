pkgname=ip-lookup
pkgver=1.1
pkgrel=1
pkgdesc="A simple IP lookup tool that shows city, ASN, and other information. Deadzone.lol Motherfucker."
arch=('x86_64')
url="https://deadzone.lol"
license=('GPL')
depends=('curl' 'cjson')
source=("main.c")
sha256sums=('SKIP')

build() {
  gcc -o "$pkgname" "$srcdir/main.c" -lcurl -lcjson
}

package() {
  install -Dm755 "$srcdir/$pkgname" "$pkgdir/usr/bin/$pkgname"
}
