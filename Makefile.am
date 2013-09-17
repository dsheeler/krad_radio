bin_PROGRAMS = krad_radio krad_radio_daemon krad_radio_vp kr_display
krad_radio_SOURCES = clients/krad_radio.c
krad_radio_CFLAGS = $(OPUS_CFLAGS)
krad_radio_LDFLAGS = lib/krad_radio/libkradclient.la 

krad_radio_daemon_SOURCES = daemon/krad_radio_daemon.c
krad_radio_daemon_CFLAGS = $(XKBCOMMON_CFLAGS) $(X11_CFLAGS) $(CAIRO_CFLAGS) $(OPUS_CFLAGS) $(XMMS2_CFLAGS)
krad_radio_daemon_LDFLAGS = lib/krad_radio/libkradclient.la lib/krad_radio/libkradradio.la  
krad_radio_daemon_LDADD = -lkradradio $(XKBCOMMON_LIBS)

krad_radio_vp_SOURCES = clients/krad_radio_vp.c
krad_radio_vp_CFLAGS = $(XKBCOMMON_CFLAGS) $(X11_CFLAGS) $(CAIRO_CFLAGS) $(OPUS_CFLAGS) $(XMMS2_CFLAGS)
krad_radio_vp_LDADD = $(CAIRO_LIBS) lib/krad_radio/libkradclient.la -lccv -lm -ljpeg -lpng -lz -lgsl -lfftw3f -lfftw3 -llinear -lgslcblas -lblas

kr_display_SOURCES = clients/kr_display.c
kr_display_CFLAGS = $(XKBCOMMON_CFLAGS) $(X11_CFLAGS) $(CAIRO_CFLAGS) $(OPUS_CFLAGS) $(XMMS2_CFLAGS)
kr_display_LDADD = lib/krad_radio/libkradradio.la lib/krad_radio/libkradclient.la $(XKBCOMMON_LIBS) -lturbojpeg

if HAVE_TURBOJPEG
krad_radio_daemon_LDADD += -lturbojpeg
endif
# Libraries

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
-Ilib/krad_fc2 \
-DKRAD_USE_X11 \
-DKRAD_USE_WAYLAND

lib_krad_radio_libkradradio_la_CXXFLAGS = -fPIC
lib_krad_radio_libkradradio_la_CFLAGS = -fPIC $(XKBCOMMON_CFLAGS) $(X11_CFLAGS) $(CAIRO_CFLAGS) $(XMMS2_CFLAGS) $(OPUS_CFLAGS)
lib_krad_radio_libkradradio_la_LIBADD = $(OPUS_LIBS) lib/krad_radio/libkradclient.la
 
lib_krad_radio_libkradradio_la_LIBADD += $(CAIRO_LIBS) $(FLAC_LIBS) $(JACK_LIBS) $(VORBIS_LIBS)\
													$(XMMS2_LIBS) $(THEORA_LIBS) $(VPX_LIBS)\
													$(LIBSWSCALE_LIBS) $(X11_LIBS) $(SAMPLERATE_LIBS) \
													$(ALSA_LIBS) $(WAYLAND_LIBS) $(XKBCOMMON_LIBS)

lib_krad_radio_libkradradio_la_LDFLAGS = -shared -no-undefined \
 -version-info @OD_LT_CURRENT@:@OD_LT_REVISION@:@OD_LT_AGE@
lib_krad_radio_libkradradio_la_LDFLAGS += 
lib_krad_radio_libkradradio_la_SOURCES = \
lib/krad_calc/krad_base64.c \
lib/krad_container/krad_container.c \
lib/krad_radio/krad_radio_interface.c \
lib/krad_radio/krad_radio.c \
lib/krad_transponder/krad_transponder_graph.c \
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
lib/krad_compositor/krad_compositor_subunit.c \
lib/krad_compositor/krad_compositor_interface.c \
lib/krad_compositor/krad_videoport.c \
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
lib/krad_sfx/krad_sfx.c \
lib/krad_app/krad_app_server.c \
lib/krad_tags/krad_tags.c \
lib/krad_mixer/krad_mixer_interface.c \
lib/krad_mixer/krad_mixer.c \
lib/krad_sfx/krad_tone.c \
lib/krad_audio/krad_audio.c \
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
lib/krad_decklink/krad_decklink_capture.cpp \
lib/krad_vhs/ext/imagezero/libiz.cpp \
lib/krad_vhs/ext/imagezero/table.cpp \
lib/krad_vhs/krad_vhs.cpp \
lib/krad_wayland/krad_wayland.c \
lib/krad_wayland/kwcap-decode.c

if LINUX
lib_krad_radio_libkradradio_la_SOURCES += lib/krad_decklink/vendor/linux/DeckLinkAPIDispatch.cpp \
																					lib/krad_v4l2/krad_v4l2.c lib/krad_alsa/krad_alsa.c \
																					lib/krad_x11/krad_x11.c
AM_CPPFLAGS += -Ilib/krad_decklink/vendor/linux 
else
AM_CPPFLAGS += -Ilib/krad_decklink/vendor/osx
endif

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
lib/krad_transponder/krad_transponder.h \
lib/krad_ebml/krad_ebml2.h \
lib/krad_ebml/krad_ebml2_ids.h \
lib/krad_ebml/krad_ebml2w.h \
lib/krad_system/krad_system.h \
lib/krad_io/krad_io2.h \
lib/krad_app/krad_app_client.h
