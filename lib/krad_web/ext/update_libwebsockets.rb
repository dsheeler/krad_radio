#!/usr/bin/env ruby

kr_nom_nom = "/home/oneman/kode/krad_radio/lib/krad_web/ext/libwebsockets"
lws_tree = "/home/oneman/kode/libwebsockets/lib"

`rm #{kr_nom_nom}/*`
`cp #{lws_tree}/base64-decode.c #{kr_nom_nom}/base64-decode.c`
`cp #{lws_tree}/extension.c #{kr_nom_nom}/extension.c`
`cp #{lws_tree}/extension-deflate-stream.c #{kr_nom_nom}/extension-deflate-stream.c`
`cp #{lws_tree}/extension-deflate-stream.h #{kr_nom_nom}/extension-deflate-stream.h`
`cp #{lws_tree}/extension-deflate-frame.c #{kr_nom_nom}/extension-deflate-frame.c`
`cp #{lws_tree}/extension-deflate-frame.h #{kr_nom_nom}/extension-deflate-frame.h`
`cp #{lws_tree}/handshake.c #{kr_nom_nom}/handshake.c`
`cp #{lws_tree}/libwebsockets.c #{kr_nom_nom}/libwebsockets.c`
`cp #{lws_tree}/libwebsockets.h #{kr_nom_nom}/libwebsockets.h`
`cp #{lws_tree}/parsers.c #{kr_nom_nom}/parsers.c`
`cp #{lws_tree}/private-libwebsockets.h #{kr_nom_nom}/private-libwebsockets.h`
`cp #{lws_tree}/output.c #{kr_nom_nom}/output.c`
`cp #{lws_tree}/server.c #{kr_nom_nom}/server.c`
`cp #{lws_tree}/server-handshake.c #{kr_nom_nom}/server-handshake.c`
`cp #{lws_tree}/sha-1.c #{kr_nom_nom}/sha-1.c`
`cp /home/oneman/kode/libwebsockets/config.h #{kr_nom_nom}/config.h`
`touch  #{kr_nom_nom}/extension-x-google-mux.h`

Dir.chdir("/home/oneman/kode/libwebsockets")
version = `git rev-parse HEAD`.chomp
Dir.chdir("/home/oneman/kode/krad_radio")

`echo #{version} > #{kr_nom_nom}/VERSION`

`echo "#define LWS_LIBRARY_VERSION PACKAGE_VERSION" >> #{kr_nom_nom}/config.h`
`echo "#define LWS_NO_CLIENT" >> #{kr_nom_nom}/config.h`
`echo "#define LWS_NO_DAEMONIZE" >> #{kr_nom_nom}/config.h`
`echo "#define assert(ignore)((void) 0)" >> #{kr_nom_nom}/config.h`

