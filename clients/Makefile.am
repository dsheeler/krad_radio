bin_PROGRAMS = kr

AM_CPPFLAGS = \
-I../lib/krad_muxponder \
-I../lib/krad_compositor \
-I../lib/krad_osc \
-I../lib/krad_v4l2 \
-I../lib/krad_flac \
-I../lib/krad_jack \
-I../lib/krad_opus \
-I../lib/krad_chronometer \
-I../lib/krad_ebml \
-I../lib/krad_system \
-I../lib/krad_y4m \
-I../lib/krad_ogg \
-I../lib/krad_audio \
-I../lib/krad_ring \
-I../lib/krad_vpx \
-I../lib/krad_alsa \
-I../lib/krad_theora \
-I../lib/krad_transmitter \
-I../lib/krad_udp \
-I../lib/krad_io \
-I../lib/krad_tags \
-I../lib/krad_list \
-I../lib/krad_legacy \
-I../lib/krad_calc \
-I../lib/krad_radio \
-I../lib/krad_player \
-I../lib/krad_vhs \
-I../lib/krad_vhs/ext \
-I../lib/krad_vhs/ext/imagezero \
-I../lib/krad_x11 \
-I../lib/krad_vorbis \
-I../lib/krad_framepool \
-I../lib/krad_transponder \
-I../lib/krad_mixer \
-I../lib/krad_app \
-I../lib/krad_coder \
-I../lib/krad_container \
-I../lib/krad_decklink \
-I../lib/krad_decklink/vendor \
-I../lib/krad_mkv \
-I../lib/krad_sfx \
-I../lib/krad_wayland \
-I../lib/krad_wayland/ext \
-I../lib/krad_web \
-I../lib/krad_web/res \
-I../lib/krad_xmms2 \
-I../lib/krad_rc \
-I../lib/krad_fc \
-I../lib/krad_mem

kr_SOURCES = kr.c
kr_CFLAGS = $(OPUS_CFLAGS)
kr_CFLAGS += -DKR_GIT_VERSION=\"$(KR_GIT_VERSION)\"
kr_LDFLAGS = ../lib/krad_radio/libkradclient.la
