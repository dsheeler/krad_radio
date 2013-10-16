AC_INIT([kradradio], m4_esyscmd([scripts/kr_version.sh]), [kradradio@gmail.com])
AC_CONFIG_SRCDIR([tests/krad_webm_dash_vod_test.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign no-define subdir-objects])
AC_CONFIG_MACRO_DIR([m4])
AC_USE_SYSTEM_EXTENSIONS
AM_MAINTAINER_MODE([enable])
LT_INIT

AM_SILENT_RULES([yes])

KR_LT_CURRENT=$PACKAGE_VERSION
KR_LT_REVISION=1
KR_LT_AGE=0

AC_SUBST(KR_LT_CURRENT)
AC_SUBST(KR_LT_REVISION)
AC_SUBST(KR_LT_AGE)

AC_SUBST(KR_GIT_VERSION, m4_esyscmd([git describe --tags]))

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
  AM_CONDITIONAL(LINUX, true)
  CFLAGS="-DKR_LINUX $CFLAGS"
  CXXFLAGS="-DKR_LINUX $CXXFLAGS"
esac


AC_CHECK_LIB([m], [dl], [z], [pthread], [rt])

AC_CHECK_LIB(turbojpeg, tjInitDecompress,
 [AM_CONDITIONAL([HAVE_TURBOJPEG], true)],
 [AM_CONDITIONAL([HAVE_TURBOJPEG], false)])

AC_CHECK_LIB(gif, DGifOpenFileName,
 [AM_CONDITIONAL([HAVE_GIF], true)],
 [AM_CONDITIONAL([HAVE_GIF], false)])

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
PKG_CHECK_MODULES([WAYLAND], [wayland-client], [AM_CONDITIONAL(HAVE_WAYLAND, true)])
PKG_CHECK_MODULES([X11], [x11,x11-xcb,xcb-image,xcb-shm],
                  [AM_CONDITIONAL(HAVE_X11, true)])
PKG_CHECK_MODULES([XMMS2], [xmms2-client])

LT_LIB_M

AC_PROG_CC
AC_PROG_CXX

AC_CONFIG_FILES([Makefile lib/Makefile daemon/Makefile clients/Makefile])
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
