#!/bin/sh

#if [ ! -f "tools/hextool/hextool" ]; then
  cd tools/code/hextool
  gcc hexutils.c hextool.c -g -o hextool
  cd ../../../
#fi

rm -f lib/krad_web/embed.h
cd lib/krad_web/core
rm -f interface.js
rm -f *.h
../../../tools/code/hextool/hextool api.js
../../../tools/code/hextool/hextool index.html
../../../tools/code/hextool/hextool dev_interface.js
cat ../rack/*.js > interface.js
../../../tools/code/hextool/hextool interface.js
cat *.h > ../embed.h
rm -f interface.js
rm -f *.h
cd ../../../
