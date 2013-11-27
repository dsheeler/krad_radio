# Maintainer: David Richards <drichards@mozilla.com>
pkgname=krad_radio
pkgver=23
pkgrel=2
pkgdesc="Krad Radio Audio Video System"
url="http://kradradio.com"
arch=('x86_64' 'i686')
license=('MPL2')
depends=('wayland' 'cairo' 'libxkbcommon' 'alsa-lib' 'jack2'
         'flac' 'opus' 'libogg' 'libsamplerate' 'libvorbis'
         'libtheora' 'libvpx' 'ffmpeg' 'xcb-util-image' 'xmms2')
source=('http://europa.kradradio.com:8040/krad_radio-23.tar.gz')
md5sums=('6e6770db6b93282dcc28d010b2117767')

build() {
  cd $pkgname-$pkgver
  ./autogen.sh
  ./configure --prefix=/usr
  make
}

package() {
  cd $pkgname-$pkgver
  make DESTDIR="$pkgdir" install
  install -Dm644 COPYING "$pkgdir/usr/share/licenses/$pkgname/COPYING"
}
