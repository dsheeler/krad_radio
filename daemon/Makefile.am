include $(top_srcdir)/common.mk

bin_PROGRAMS = krad_radio

krad_radio_SOURCES = krad_radio.c
krad_radio_CFLAGS = $(XKBCOMMON_CFLAGS) $(X11_CFLAGS) $(CAIRO_CFLAGS) $(OPUS_CFLAGS) $(XMMS2_CFLAGS)
krad_radio_LDFLAGS = ../lib/krad_radio/libkradclient.la ../lib/krad_radio/libkradradio.la
krad_radio_CFLAGS += -DKR_GIT_VERSION=\"$(KR_GIT_VERSION)\"
