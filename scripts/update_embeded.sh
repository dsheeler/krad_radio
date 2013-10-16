#!/bin/sh

cd lib/krad_web/core
rm -f kr_api.js.h
../../../tools/hextool/hextool kr_api.js
rm -f krad_radio.html.h
../../../tools/hextool/hextool krad_radio.html
rm -f kr_dev_interface.js.h
../../../tools/hextool/hextool kr_dev_interface.js

cd ../rack
rm -f kr_rack.js
rm -f kr_rack.js.h
cat *.js > kr_rack.js
../../../tools/hextool/hextool kr_rack.js
rm -f kr_rack.js
cd ../../../
