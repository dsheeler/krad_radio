#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([kradradio], [21], [kradradio@gmail.com])
AC_CONFIG_SRCDIR([tests/krad_webm_dash_vod_test.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.11 foreign no-define subdir-objects])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
CFLAGS="$CFLAGS -Wall"
CXXFLAGS="$CXXFLAGS -Wall"
CPPFLAGS="$CPPFLAGS -Wall"

# Checks for libraries.
AC_CHECK_LIB(m, sin, [], AC_MSG_ERROR([library m not found]))
AC_CHECK_LIB(dl, dlopen, [], AC_MSG_ERROR([library dl not found]))
AC_CHECK_LIB(z, compress, [], AC_MSG_ERROR([library z not found]))
AC_CHECK_LIB(pthread, pthread_create, [], AC_MSG_ERROR([library pthread not found]))
PKG_CHECK_MODULES([NCURSES], [ncurses])
PKG_CHECK_MODULES([PANEL], [panel])
PKG_CHECK_MODULES([MENU], [menu])
PKG_CHECK_MODULES([FORM], [form])
PKG_CHECK_MODULES([CAIRO], [cairo])
PKG_CHECK_MODULES([CAIRO_FT], [cairo-ft])
PKG_CHECK_MODULES([NCURSES], [ncurses])
PKG_CHECK_MODULES([WAYLAND_CLIENT], [wayland-client])
PKG_CHECK_MODULES([XKBCOMMON], [xkbcommon])
PKG_CHECK_MODULES([JACK], [jack])
PKG_CHECK_MODULES([NCURSES], [ncurses])
PKG_CHECK_MODULES([XMMS2_CLIENT], [xmms2-client])
PKG_CHECK_MODULES([SAMPLERATE], [samplerate])
PKG_CHECK_MODULES([THEORA], [theora])
PKG_CHECK_MODULES([THEORAENC], [theoraenc])
PKG_CHECK_MODULES([THEORADEC], [theoradec])
PKG_CHECK_MODULES([VORBIS], [vorbis])
PKG_CHECK_MODULES([VORBISENC], [vorbisenc])
PKG_CHECK_MODULES([FLAC], [flac])
PKG_CHECK_MODULES([VPX], [vpx])
PKG_CHECK_MODULES([LIBSWSCALE], [libswscale])
PKG_CHECK_MODULES([LIBAVUTIL], [libavutil])
PKG_CHECK_MODULES([FREETYPE2], [freetype2])
PKG_CHECK_MODULES([X11_XCB], [x11-xcb])
PKG_CHECK_MODULES([XCB_IMAGE], [xcb-image])
PKG_CHECK_MODULES([XCB_SHM], [xcb-shm])
PKG_CHECK_MODULES([XCB], [xcb])
PKG_CHECK_MODULES([X11], [x11])
PKG_CHECK_MODULES([ALSA], [alsa])


# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h limits.h locale.h mach/mach.h malloc.h memory.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/ioctl.h sys/mount.h sys/param.h sys/socket.h sys/statfs.h sys/time.h termios.h unistd.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([bzero clock_gettime floor ftruncate gettimeofday inet_ntoa memmove memset munmap pow setlocale socket sqrt strchr strcspn strdup strerror strndup strrchr strstr strtol strtoul uname])

# AC_CONFIG_SUBDIRS([lib/krad_vhs/ext/imagezero])
AC_CONFIG_FILES([Makefile]) 
AC_OUTPUT

AC_MSG_RESULT([
------------------------------------------------------------------------
  $PACKAGE_NAME $PACKAGE_VERSION:  Automatic configuration OK.
------------------------------------------------------------------------
])

echo "Type \"make\" to compile";
echo "Type \"make install\" (as root) to install";
echo "Type \"make uninstall\" (as root) to uninstall";
