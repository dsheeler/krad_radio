include $(top_srcdir)/common.mk

lib_LTLIBRARIES = krad_radio/libkradclient.la krad_radio/libkradradio.la

krad_radio_libkradradio_la_CXXFLAGS =
krad_radio_libkradradio_la_CFLAGS = $(XKBCOMMON_CFLAGS) $(X11_CFLAGS) $(CAIRO_CFLAGS) $(XMMS2_CFLAGS) $(OPUS_CFLAGS)
krad_radio_libkradradio_la_LIBADD = $(OPUS_LIBS) krad_radio/libkradclient.la

krad_radio_libkradradio_la_LIBADD += $(CAIRO_LIBS) $(FLAC_LIBS) $(JACK_LIBS) $(VORBIS_LIBS)\
													$(XMMS2_LIBS) $(THEORA_LIBS) $(VPX_LIBS)\
													$(LIBSWSCALE_LIBS) $(X11_LIBS) $(SAMPLERATE_LIBS) \
													$(ALSA_LIBS) $(WAYLAND_LIBS) $(XKBCOMMON_LIBS)

krad_radio_libkradradio_la_LDFLAGS = -shared -no-undefined \
 -version-info @KR_LT_CURRENT@:@KR_LT_REVISION@:@KR_LT_AGE@
krad_radio_libkradradio_la_LDFLAGS +=
krad_radio_libkradradio_la_SOURCES = \
krad_calc/krad_base64.c \
krad_mem/krad_pool.c \
krad_container/krad_container.c \
krad_radio/krad_radio_interface.c \
krad_radio/krad_radio.c \
krad_transponder/krad_adapter.c \
krad_transponder/krad_slice.c \
krad_chronometer/krad_timer.c \
krad_chronometer/krad_ticker.c \
krad_coder/krad_coder_common.c \
krad_coder/krad_encoder.c \
krad_y4m/krad_y4m.c \
krad_mkv/krad_mkv.c \
krad_mkv/krad_mkv_demux.c \
krad_io/krad_stream.c \
krad_io/krad_file.c \
krad_compositor/krad_convert.c \
krad_compositor/krad_compositor_interface.c \
krad_compositor/krad_vector.c \
krad_compositor/krad_sprite.c \
krad_compositor/krad_text.c \
krad_chronometer/krad_easing.c \
krad_compositor/krad_perspective.c \
krad_compositor/krad_image_pool.c \
krad_compositor/krad_compositor.c \
krad_framepool/krad_framepool.c \
krad_osc/krad_osc.c \
krad_calc/krad_sha1.c \
krad_sfx/krad_eq.c \
krad_sfx/krad_analog.c \
krad_sfx/krad_pass.c \
krad_sfx/krad_hardlimiter.c \
krad_sfx/krad_sfx_common.c \
krad_sfx/krad_sfx.c \
krad_app/krad_app_server.c \
krad_tags/krad_tags.c \
krad_mixer/krad_mixer_interface.c \
krad_mixer/krad_mixer.c \
krad_sfx/krad_tone.c \
krad_ring/krad_resample_ring.c \
krad_vpx/krad_vpx.c \
krad_theora/krad_theora.c \
krad_vorbis/krad_vorbis.c \
krad_flac/krad_flac.c \
krad_opus/krad_opus.c \
krad_opus/opus_header.c \
krad_ring/krad_ring.c \
krad_transponder/krad_transponder_server.c \
krad_transponder/krad_transponder.c \
krad_web/krad_interweb.c \
krad_transmitter/krad_transmitter.c \
krad_ogg/krad_ogg.c \
krad_ogg/krad_ogg2.c \
krad_udp/krad_udp.c

if HAVE_JACK
krad_radio_libkradradio_la_SOURCES += krad_jack/krad_jack.c
AM_CPPFLAGS += -DKR_JACK
endif

if HAVE_XMMS2
krad_radio_libkradradio_la_SOURCES += krad_xmms2/krad_xmms2.c
AM_CPPFLAGS += -DKR_XMMS2
endif

if HAVE_WAYLAND
krad_radio_libkradradio_la_SOURCES += krad_wayland/krad_wayland.c
AM_CPPFLAGS += -DKR_WAYLAND
endif

if HAVE_X11
krad_radio_libkradradio_la_SOURCES += krad_x11/krad_x11.c
AM_CPPFLAGS += -DKR_X11
endif

if HAVE_TURBOJPEG
krad_radio_libkradradio_la_LIBADD += -lturbojpeg
AM_CPPFLAGS += -DKR_TURBOJPEG
endif

if HAVE_GIF
krad_radio_libkradradio_la_LIBADD += -lgif
AM_CPPFLAGS += -DKR_GIF
endif

if KR_DECKLINK
krad_radio_libkradradio_la_SOURCES += krad_decklink/vendor/linux/DeckLinkAPIDispatch.cpp \
 krad_decklink/krad_decklink.c krad_decklink/krad_decklink_capture.cpp
AM_CPPFLAGS += -DKR_DECKLINK
if KR_LINUX
AM_CPPFLAGS += -Ikrad_decklink/vendor/linux
else
AM_CPPFLAGS += -Ikrad_decklink/vendor/osx
endif
endif

if KR_ALSA
krad_radio_libkradradio_la_SOURCES += krad_alsa/krad_alsa.c
AM_CPPFLAGS += -DKR_ALSA
endif

if KR_V4L2
krad_radio_libkradradio_la_SOURCES += krad_v4l2/krad_v4l2.c
AM_CPPFLAGS += -DKR_V4L2
endif

krad_radio_libkradclient_la_CFLAGS = $(OPUS_CFLAGS)
krad_radio_libkradclient_la_LIBADD = $(OPUS_LIBS) -lpthread
krad_radio_libkradclient_la_LDFLAGS = -shared -no-undefined \
 -version-info @KR_LT_CURRENT@:@KR_LT_REVISION@:@KR_LT_AGE@
krad_radio_libkradclient_la_LDFLAGS +=
krad_radio_libkradclient_la_SOURCES = \
krad_ebml/krad_ebml.c \
krad_ebml/krad_ebmlx.c \
krad_io/krad_io.c \
krad_system/krad_system.c \
krad_app/krad_app_client.c \
krad_radio/krad_radio_client.c \
krad_radio/krad_radio_client_ctl.c \
krad_mixer/krad_mixer_client.c \
krad_compositor/krad_compositor_client.c \
krad_transponder/krad_transponder_client.c \
krad_radio/krad_radio_common.c \
krad_mixer/krad_mixer_common.c \
krad_compositor/krad_compositor_common.c \
krad_transponder/krad_transponder_common.c \
krad_sfx/krad_sfx_common.c \
gen/gen.c

kradradioincludedir = $(includedir)/krad
kradradioinclude_HEADERS = krad_radio/kr_client.h \
krad_radio/krad_radio_client.h \
krad_radio/krad_radio_client_internal.h \
krad_radio/krad_radio_ipc.h \
krad_radio/krad_radio_client_ctl.h \
krad_radio/krad_radio_common.h \
krad_radio/krad_radio_version.h \
krad_mixer/krad_mixer_client.h \
krad_mixer/krad_mixer_common.h \
krad_compositor/krad_compositor_client.h \
krad_compositor/krad_compositor_common.h \
krad_transponder/krad_transponder_client.h \
krad_transponder/krad_transponder_common.h \
krad_ebml/krad_ebml.h \
krad_ebml/krad_ebml_ids.h \
krad_ebml/krad_ebmlx.h \
krad_system/krad_system.h \
krad_io/krad_io.h \
krad_app/krad_app_client.h

.PHONY: all
