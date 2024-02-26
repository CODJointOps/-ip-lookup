pkgname=ip-lookup
pkgver=1.2
pkgrel=1
pkgdesc="A simple IP lookup tool that shows city, ASN, and other information. Deadzone.lol Motherfucker."
arch=('x86_64')
url="https://deadzone.lol"
license=('GPL')
depends=('curl' 'cjson')

build() {
  cd ..
  make
}

package() {
  cd ..
  install -Dm755 "./bin/$pkgname" "$pkgdir/usr/bin/$pkgname"
}

