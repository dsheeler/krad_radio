# Libraries

lib_LTLIBRARIES = lib/krad_radio/libkradclient.la

AM_CPPFLAGS = \
-Ilib/krad_muxponder \
-Ilib/krad_compositor \
-Ilib/krad_osc \
-Ilib/krad_v4l2 \
-Ilib/krad_flac \
-Ilib/krad_jack \
-Ilib/krad_opus \
-Ilib/krad_chronometer \
-Ilib/krad_ebml \
-Ilib/krad_system \
-Ilib/krad_y4m \
-Ilib/krad_ogg \
-Ilib/krad_audio \
-Ilib/krad_ring \
-Ilib/krad_vpx \
-Ilib/krad_alsa \
-Ilib/krad_theora \
-Ilib/krad_transmitter \
-Ilib/krad_udp \
-Ilib/krad_io \
-Ilib/krad_tags \
-Ilib/krad_list \
-Ilib/krad_legacy \
-Ilib/krad_calc \
-Ilib/krad_radio \
-Ilib/krad_player \
-Ilib/krad_vhs \
-Ilib/krad_vhs/ext \
-Ilib/krad_vhs/ext/imagezero \
-Ilib/krad_x11 \
-Ilib/krad_vorbis \
-Ilib/krad_framepool \
-Ilib/krad_transponder \
-Ilib/krad_mixer \
-Ilib/krad_app \
-Ilib/krad_coder \
-Ilib/krad_container \
-Ilib/krad_decklink \
-Ilib/krad_decklink/vendor \
-Ilib/krad_decklink/vendor/osx \
-Ilib/krad_decklink/vendor/linux \
-Ilib/krad_mkv \
-Ilib/krad_sfx \
-Ilib/krad_wayland \
-Ilib/krad_wayland/ext \
-Ilib/krad_web \
-Ilib/krad_web/res \
-Ilib/krad_xmms2 \
-Ilib/krad_rc \
-Ilib/krad_fc2

lib_krad_radio_libkradclient_la_CFLAGS = $(OPUS_CFLAGS)
lib_krad_radio_libkradclient_la_LIBADD = $(OPUS_LIBS)
lib_krad_radio_libkradclient_la_LDFLAGS = -shared -no-undefined \
 -version-info @OD_LT_CURRENT@:@OD_LT_REVISION@:@OD_LT_AGE@
lib_krad_radio_libkradclient_la_LDFLAGS += 
lib_krad_radio_libkradclient_la_SOURCES = \
lib/krad_ebml/krad_ebml2.c \
lib/krad_ebml/krad_ebml2w.c \
lib/krad_io/krad_io2.c \
lib/krad_system/krad_system.c \
lib/krad_app/krad_app_client.c \
lib/krad_radio/krad_radio_client.c \
lib/krad_radio/krad_radio_client_ctl.c \
lib/krad_mixer/krad_mixer_client.c \
lib/krad_compositor/krad_compositor_client.c \
lib/krad_transponder/krad_transponder_client.c \
lib/krad_radio/krad_radio_common.c \
lib/krad_mixer/krad_mixer_common.c \
lib/krad_compositor/krad_compositor_common.c \
lib/krad_transponder/krad_transponder_common.c

