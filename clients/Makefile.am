include $(top_srcdir)/common.mk

bin_PROGRAMS = kr
noinst_PROGRAMS = kr_example_client

kr_SOURCES = kr.c
kr_CFLAGS = $(OPUS_CFLAGS)
kr_CFLAGS += -DKR_GIT_VERSION=\"$(KR_GIT_VERSION)\"
kr_LDFLAGS = ../lib/krad_radio/libkradclient.la

kr_example_client_SOURCES = kr_example_client.c
kr_example_client_CFLAGS = $(OPUS_CFLAGS)
kr_example_client_CFLAGS += -DKR_GIT_VERSION=\"$(KR_GIT_VERSION)\"
kr_example_client_LDFLAGS = ../lib/krad_radio/libkradclient.la
