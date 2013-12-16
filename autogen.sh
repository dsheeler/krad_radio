#!/bin/sh
set -e

srcdir=`dirname $0`
test -n "$srcdir" && cd "$srcdir"

if [ "$1" = "--clean" ]; then
  if [ -e "Makefile" ]; then
    make clean
  fi
  rm -rf autom4te.cache m4
  rm -f depcomp config.guess config.sub libtool ltmain.sh\
   config.status Makefile stamp-h1 config.h.in~ config.h\
   config.log missing INSTALL Makefile.in NEWS aclocal.m4\
   compile config.h.in config.log configure missing install-sh\
   kr krad_radio
  echo "Cleaned up autojunk"
  exit
fi

echo "Updating build configuration files for krad radio..."

ACLOCAL_FLAGS="-I m4"
autoreconf -if
