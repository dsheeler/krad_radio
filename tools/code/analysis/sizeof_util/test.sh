#!/bin/sh

$(echo $(./sizeof_util ../../lib/ "kr_" "_info" out.c "-I../../lib/krad_radio
-I../../lib/krad_system -I../../lib/krad_chronometer -I../../lib/krad_tags
-I../../lib/krad_ebml -I../../lib/krad_app -I../../lib/krad_ring
-I../../lib/krad_io -I../../lib/krad_mixer -I../../lib/krad_sfx
-I../../lib/krad_compositor -I../../lib/krad_transponder -I../../lib/krad_mem
-I/usr/include/opus -I../../lib/krad_container -I../../lib/krad_jack
-I../../lib/krad_wayland -I../../lib/krad_v4l2 -I../../lib/krad_framepool
-I/usr/include/freetype2 -I/usr/include/cairo -I../../lib/krad_web -I../../lib/krad_calc
-I../../lib/krad_osc  -I../../lib/krad_alsa -I../../lib/krad_coder
-I../../lib/krad_player -I../../lib/krad_mkv -I../../lib/krad_transmitter
-I../../lib/krad_ogg -I../../lib/krad_udp -I../../lib/krad_theora
-I../../lib/krad_vpx -I../../lib/krad_vorbis -I../../lib/krad_flac
-I../../lib/krad_opus -DKR_LINUX")) && ./out ;  rm -f out ; rm -f out.c
