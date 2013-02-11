`rm /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/*`
`cp /home/oneman/kode/libwebsockets/lib/base64-decode.c /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/base64-decode.c`
`cp /home/oneman/kode/libwebsockets/lib/extension.c /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/extension.c`
`cp /home/oneman/kode/libwebsockets/lib/extension-deflate-stream.c /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/extension-deflate-stream.c`
`cp /home/oneman/kode/libwebsockets/lib/extension-deflate-stream.h /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/extension-deflate-stream.h`
`cp /home/oneman/kode/libwebsockets/lib/extension-deflate-frame.c /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/extension-deflate-frame.c`
`cp /home/oneman/kode/libwebsockets/lib/extension-deflate-frame.h /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/extension-deflate-frame.h`
`cp /home/oneman/kode/libwebsockets/lib/handshake.c /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/handshake.c`
`cp /home/oneman/kode/libwebsockets/lib/libwebsockets.c /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/libwebsockets.c`
`cp /home/oneman/kode/libwebsockets/lib/libwebsockets.h /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/libwebsockets.h`
`cp /home/oneman/kode/libwebsockets/lib/parsers.c /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/parsers.c`
`cp /home/oneman/kode/libwebsockets/lib/private-libwebsockets.h /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/private-libwebsockets.h`
`cp /home/oneman/kode/libwebsockets/lib/output.c /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/output.c`
`cp /home/oneman/kode/libwebsockets/lib/server.c /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/server.c`
`cp /home/oneman/kode/libwebsockets/lib/server-handshake.c /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/server-handshake.c`
`cp /home/oneman/kode/libwebsockets/lib/sha-1.c /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/sha-1.c`
`cp /home/oneman/kode/libwebsockets/config.h /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/config.h`
`touch  /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/extension-x-google-mux.h`

Dir.chdir("/home/oneman/kode/libwebsockets")
version = `git rev-parse HEAD`
Dir.chdir("/home/oneman/kode/krad_radio")
`echo #{version} > /home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets/VERSION`
