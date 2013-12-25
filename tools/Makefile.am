include $(top_srcdir)/common.mk

bin_PROGRAMS = kr_dl_stream kr_v4l2_stream kr_x11_stream

kr_dl_stream_SOURCES = kr_dl_stream.c
kr_dl_stream_CFLAGS = $(OPUS_CFLAGS)
kr_dl_stream_CFLAGS += -DKR_GIT_VERSION=\"$(KR_GIT_VERSION)\"
kr_dl_stream_LDFLAGS = ../lib/krad_radio/libkradclient.la ../lib/krad_radio/libkradradio.la

kr_v4l2_stream_SOURCES = kr_v4l2_stream.c
kr_v4l2_stream_CFLAGS = $(OPUS_CFLAGS)
kr_v4l2_stream_CFLAGS += -DKR_GIT_VERSION=\"$(KR_GIT_VERSION)\"
kr_v4l2_stream_LDFLAGS = ../lib/krad_radio/libkradclient.la ../lib/krad_radio/libkradradio.la

kr_x11_stream_SOURCES = kr_x11_stream.c
kr_x11_stream_CFLAGS = $(OPUS_CFLAGS)
kr_x11_stream_CFLAGS += -DKR_GIT_VERSION=\"$(KR_GIT_VERSION)\"
kr_x11_stream_LDFLAGS = ../lib/krad_radio/libkradclient.la ../lib/krad_radio/libkradradio.la
