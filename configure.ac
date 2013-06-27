# autoconf source script for generating configure

#FIXME unify version number nonsense
AC_INIT([kradradio], [22], [kradradio@gmail.com])
AC_CONFIG_SRCDIR([tests/krad_webm_dash_vod_test.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign no-define subdir-objects])

AM_MAINTAINER_MODE([enable])
LT_INIT

dnl Library versioning for libtool.
dnl Please update these for releases.
dnl CURRENT, REVISION, AGE
dnl - library source changed -> increment REVISION
dnl - interfaces added/removed/changed -> increment CURRENT, REVISION = 0
dnl - interfaces added -> increment AGE
dnl - interfaces removed -> AGE = 0

OD_LT_CURRENT=22
OD_LT_REVISION=1
OD_LT_AGE=0

AC_SUBST(OD_LT_CURRENT)
AC_SUBST(OD_LT_REVISION)
AC_SUBST(OD_LT_AGE)

CFLAGS="-g -Wall $CFLAGS"
CXXFLAGS="-g -Wall $CXXFLAGS"

AC_ARG_ENABLE([optimize],
  AS_HELP_STRING([--enable-optimize], [Enable -O3]),,
  enable_optimize=no)

AS_IF([test "$enable_optimize" = "yes"], [
  CFLAGS="-O3 $CFLAGS"
  CXXFLAGS="-O3 $CXXFLAGS"
])

case $host in
  *-linux*)
  CFLAGS="-DKR_LINUX $CFLAGS"
  CXXFLAGS="-DKR_LINUX $CXXFLAGS"
esac

# Checks for libraries.
AC_CHECK_LIB(m, sin, [], AC_MSG_ERROR([library m not found]))
AC_CHECK_LIB(dl, dlopen, [], AC_MSG_ERROR([library dl not found]))
AC_CHECK_LIB(z, compress, [], AC_MSG_ERROR([library z not found]))
AC_CHECK_LIB(pthread, pthread_create, [], AC_MSG_ERROR([pthread not found]))
AC_CHECK_LIB(rt, clock_gettime, [], AC_MSG_ERROR([library rt not found]))
AC_CHECK_LIB(turbojpeg, tjInitDecompress, [], [])
AC_CHECK_LIB(gif, DGifOpenFileName, [], [])

PKG_CHECK_MODULES([HOBGOBLINFIRECAKE], [hobgoblinfirecake],
   [AC_DEFINE([HAVE_HOBGOBLINFIRECAKE], [1], [Use HOBGOBLINFIRECAKE])],
   [PKG_CHECK_MODULES([ALSA], [alsa],
       [AC_DEFINE([HAVE_ALSA], [1], [Use ALSA])
    ])
])
PKG_CHECK_MODULES([JACK], [jack])
PKG_CHECK_MODULES([OPUS], [opus])
PKG_CHECK_MODULES([VORBIS], [vorbis,vorbisenc])
PKG_CHECK_MODULES([FLAC], [flac])
PKG_CHECK_MODULES([THEORA], [theora,theoraenc,theoradec])
PKG_CHECK_MODULES([VPX], [vpx])
PKG_CHECK_MODULES([SAMPLERATE], [samplerate])
PKG_CHECK_MODULES([LIBSWSCALE], [libswscale])
PKG_CHECK_MODULES([AVUTIL], [libavutil])
PKG_CHECK_MODULES([FREETYPE2], [freetype2])
PKG_CHECK_MODULES([CAIRO], [cairo,cairo-ft])
PKG_CHECK_MODULES([NCURSES], [ncurses,panel,menu,form])
PKG_CHECK_MODULES([XKBCOMMON], [xkbcommon])
PKG_CHECK_MODULES([WAYLAND], [wayland-client])
PKG_CHECK_MODULES([X11], [x11,x11-xcb,xcb-image,xcb-shm])
PKG_CHECK_MODULES([XMMS2], [xmms2-client])

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
