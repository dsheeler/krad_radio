#/bin/sh

cloc --exclude-dir=doc,scripts,test,lib/krad_web/ext/libwebsockets,lib/krad_vhs/ext,\
lib/krad_decklink/vendor,lib/krad_web/res \
--exclude-ext=rb,sh \
clients daemon lib
