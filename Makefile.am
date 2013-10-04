lib_LTLIBRARIES = lib/krad_radio/libkradclient.la lib/krad_radio/libkradradio.la

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
-Ilib/krad_mkv \
-Ilib/krad_sfx \
-Ilib/krad_wayland \
-Ilib/krad_wayland/ext \
-Ilib/krad_web \
-Ilib/krad_web/res \
-Ilib/krad_xmms2 \
-Ilib/krad_rc \
-Ilib/krad_fc \
-Ilib/krad_mem

lib_krad_radio_libkradradio_la_CXXFLAGS =
lib_krad_radio_libkradradio_la_CFLAGS = $(XKBCOMMON_CFLAGS) $(X11_CFLAGS) $(CAIRO_CFLAGS) $(XMMS2_CFLAGS) $(OPUS_CFLAGS)
lib_krad_radio_libkradradio_la_LIBADD = $(OPUS_LIBS) lib/krad_radio/libkradclient.la

lib_krad_radio_libkradradio_la_LIBADD += $(CAIRO_LIBS) $(FLAC_LIBS) $(JACK_LIBS) $(VORBIS_LIBS)\
													$(XMMS2_LIBS) $(THEORA_LIBS) $(VPX_LIBS)\
													$(LIBSWSCALE_LIBS) $(X11_LIBS) $(SAMPLERATE_LIBS) \
													$(ALSA_LIBS) $(WAYLAND_LIBS) $(XKBCOMMON_LIBS)

lib_krad_radio_libkradradio_la_LDFLAGS = -shared -no-undefined \
 -version-info @KR_LT_CURRENT@:@KR_LT_REVISION@:@KR_LT_AGE@
lib_krad_radio_libkradradio_la_LDFLAGS +=
lib_krad_radio_libkradradio_la_SOURCES = \
lib/krad_calc/krad_base64.c \
lib/krad_mem/krad_pool.c \
lib/krad_container/krad_container.c \
lib/krad_radio/krad_radio_interface.c \
lib/krad_radio/krad_radio.c \
lib/krad_transponder/krad_adapter.c \
lib/krad_transponder/krad_slice.c \
lib/krad_chronometer/krad_timer.c \
lib/krad_chronometer/krad_ticker.c \
lib/krad_coder/krad_coder_common.c \
lib/krad_coder/krad_encoder.c \
lib/krad_y4m/krad_y4m.c \
lib/krad_xmms2/krad_xmms2.c \
lib/krad_mkv/krad_mkv.c \
lib/krad_mkv/krad_mkv_demux.c \
lib/krad_io/krad_stream.c \
lib/krad_io/krad_file.c \
lib/krad_compositor/krad_convert.c \
lib/krad_compositor/krad_compositor_interface.c \
lib/krad_compositor/krad_vector.c \
lib/krad_compositor/krad_sprite.c \
lib/krad_compositor/krad_text.c \
lib/krad_chronometer/krad_easing.c \
lib/krad_compositor/krad_perspective.c \
lib/krad_compositor/krad_compositor.c \
lib/krad_framepool/krad_framepool.c \
lib/krad_osc/krad_osc.c \
lib/krad_calc/krad_sha1.c \
lib/krad_sfx/krad_eq.c \
lib/krad_sfx/krad_analog.c \
lib/krad_sfx/krad_pass.c \
lib/krad_sfx/krad_hardlimiter.c \
lib/krad_sfx/krad_sfx_common.c \
lib/krad_sfx/krad_sfx.c \
lib/krad_app/krad_app_server.c \
lib/krad_tags/krad_tags.c \
lib/krad_mixer/krad_mixer_interface.c \
lib/krad_mixer/krad_mixer.c \
lib/krad_sfx/krad_tone.c \
lib/krad_jack/krad_jack.c \
lib/krad_ring/krad_resample_ring.c \
lib/krad_vpx/krad_vpx.c \
lib/krad_theora/krad_theora.c \
lib/krad_vorbis/krad_vorbis.c \
lib/krad_flac/krad_flac.c \
lib/krad_opus/krad_opus.c \
lib/krad_opus/opus_header.c \
lib/krad_ring/krad_ring.c \
lib/krad_transponder/krad_transponder_interface.c \
lib/krad_transponder/krad_transponder.c \
lib/krad_web/krad_interweb.c \
lib/krad_transmitter/krad_transmitter.c \
lib/krad_ogg/krad_ogg.c \
lib/krad_ogg/krad_ogg2.c \
lib/krad_udp/krad_udp.c \
lib/krad_decklink/krad_decklink.c \
lib/krad_decklink/krad_decklink_capture.cpp

if HAVE_WAYLAND
lib_krad_radio_libkradradio_la_SOURCES += lib/krad_wayland/krad_wayland.c

AM_CPPFLAGS += -DKRAD_USE_WAYLAND
endif

if HAVE_X11
lib_krad_radio_libkradradio_la_SOURCES += lib/krad_x11/krad_x11.c

AM_CPPFLAGS += -DKRAD_USE_X11
endif

if HAVE_TURBOJPEG
lib_krad_radio_libkradradio_la_LIBADD += -lturbojpeg
endif

if HAVE_GIF
lib_krad_radio_libkradradio_la_LIBADD += -lgif
AM_CPPFLAGS += -DKRAD_GIF
endif

if LINUX
lib_krad_radio_libkradradio_la_SOURCES += lib/krad_decklink/vendor/linux/DeckLinkAPIDispatch.cpp \
																					lib/krad_v4l2/krad_v4l2.c lib/krad_alsa/krad_alsa.c
AM_CPPFLAGS += -Ilib/krad_decklink/vendor/linux
else
AM_CPPFLAGS += -Ilib/krad_decklink/vendor/osx
endif

lib_krad_radio_libkradclient_la_CFLAGS = $(OPUS_CFLAGS)
lib_krad_radio_libkradclient_la_LIBADD = $(OPUS_LIBS) -lpthread
lib_krad_radio_libkradclient_la_LDFLAGS = -shared -no-undefined \
 -version-info @KR_LT_CURRENT@:@KR_LT_REVISION@:@KR_LT_AGE@
lib_krad_radio_libkradclient_la_LDFLAGS +=
lib_krad_radio_libkradclient_la_SOURCES = \
lib/krad_ebml/krad_ebml.c \
lib/krad_ebml/krad_ebmlx.c \
lib/krad_io/krad_io.c \
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
lib/krad_transponder/krad_transponder_common.c \
lib/krad_sfx/krad_sfx_common.c

kradradioincludedir = $(includedir)/krad
kradradioinclude_HEADERS = lib/krad_radio/kr_client.h \
lib/krad_radio/krad_radio_client.h \
lib/krad_radio/krad_radio_client_internal.h \
lib/krad_radio/krad_radio_ipc.h \
lib/krad_radio/krad_radio_client_ctl.h \
lib/krad_radio/krad_radio_common.h \
lib/krad_radio/krad_radio_version.h \
lib/krad_mixer/krad_mixer_client.h \
lib/krad_mixer/krad_mixer_common.h \
lib/krad_compositor/krad_compositor_client.h \
lib/krad_compositor/krad_compositor_common.h \
lib/krad_transponder/krad_transponder_client.h \
lib/krad_transponder/krad_transponder_common.h \
lib/krad_ebml/krad_ebml.h \
lib/krad_ebml/krad_ebml_ids.h \
lib/krad_ebml/krad_ebmlx.h \
lib/krad_system/krad_system.h \
lib/krad_io/krad_io.h \
lib/krad_app/krad_app_client.h

bin_PROGRAMS = kr krad_radio
kr_SOURCES = clients/kr.c
kr_CFLAGS = $(OPUS_CFLAGS)
kr_LDFLAGS = lib/krad_radio/libkradclient.la

krad_radio_SOURCES = daemon/krad_radio.c
krad_radio_CFLAGS = $(XKBCOMMON_CFLAGS) $(X11_CFLAGS) $(CAIRO_CFLAGS) $(OPUS_CFLAGS) $(XMMS2_CFLAGS)
krad_radio_LDFLAGS = lib/krad_radio/libkradclient.la lib/krad_radio/libkradradio.la
#krad_radio_LDADD = -lkradradio $(XKBCOMMON_LIBS)

kr_CFLAGS += -DKR_GIT_VERSION=\"$(KR_GIT_VERSION)\"
krad_radio_CFLAGS += -DKR_GIT_VERSION=\"$(KR_GIT_VERSION)\"
