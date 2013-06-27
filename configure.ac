# autoconf source script for generating configure

#FIXME unify version number nonsense
AC_INIT([kradradio], [22], [kradradio@gmail.com])
AC_CONFIG_SRCDIR([tests/krad_webm_dash_vod_test.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign no-define subdir-objects])

AM_MAINTAINER_MODE([enable])
#LT_INIT

CFLAGS="-g -Wall $CFLAGS"
CXXFLAGS="-g -Wall $CXXFLAGS"

AC_ARG_ENABLE([optimize],
  AS_HELP_STRING([--enable-optimize], [Enable -O3]),,
  enable_optimize=no)

AS_IF([test "$enable_optimize" = "yes"], [
  CFLAGS="-O3 $CFLAGS"
  CXXFLAGS="-O3 $CXXFLAGS"
])

# Checks for libraries.
AC_CHECK_LIB(m, sin, [], AC_MSG_ERROR([library m not found]))
AC_CHECK_LIB(dl, dlopen, [], AC_MSG_ERROR([library dl not found]))
AC_CHECK_LIB(z, compress, [], AC_MSG_ERROR([library z not found]))
AC_CHECK_LIB(pthread, pthread_create, [], AC_MSG_ERROR([pthread not found]))
AC_CHECK_LIB(turbojpeg, tjInitDecompress,
 [], AC_MSG_ERROR([turbojpeg not found]))

PKG_CHECK_MODULES([ncurses], [ncurses,panel,menu,form])
PKG_CHECK_MODULES([ALSA], [alsa])
PKG_CHECK_MODULES([JACK], [jack])
PKG_CHECK_MODULES([XMMS2], [xmms2-client])
PKG_CHECK_MODULES([Theora], [theora,theoraenc,theoradec])
PKG_CHECK_MODULES([Vorbis], [vorbis,vorbisenc])
PKG_CHECK_MODULES([FLAC], [flac])
PKG_CHECK_MODULES([VPX], [vpx])
PKG_CHECK_MODULES([Secret_Rabbit_Code], [samplerate])
PKG_CHECK_MODULES([software_scaler], [libswscale])
PKG_CHECK_MODULES([avutil], [libavutil])
PKG_CHECK_MODULES([Freetype_2], [freetype2])
PKG_CHECK_MODULES([Cairo], [cairo,cairo-ft])
PKG_CHECK_MODULES([Wayland], [wayland-client])
PKG_CHECK_MODULES([xkbcommon], [xkbcommon])
PKG_CHECK_MODULES([X11], [x11,x11-xcb,xcb-image,xcb-shm])

# AC_CONFIG_SUBDIRS([lib/krad_vhs/ext/imagezero])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

AC_MSG_RESULT([
------------------------------------------------------------------------
  $PACKAGE_NAME $PACKAGE_VERSION: Configuration Valid.
  CFLAGS: $CFLAGS
------------------------------------------------------------------------
])

echo "Type \"make\" to compile";
echo "Type \"make install\" (as root) to install";
echo "Type \"make uninstall\" (as root) to uninstall";
