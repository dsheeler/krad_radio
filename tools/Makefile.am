include $(top_srcdir)/common.mk

bin_PROGRAMS = kr_dl_stream

kr_dl_stream_SOURCES = kr_dl_stream.c
kr_dl_stream_CFLAGS = $(OPUS_CFLAGS)
kr_dl_stream_CFLAGS += -DKR_GIT_VERSION=\"$(KR_GIT_VERSION)\"
kr_dl_stream_LDFLAGS = ../lib/krad_radio/libkradclient.la ../lib/krad_radio/libkradradio.la
