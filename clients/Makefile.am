include $(top_srcdir)/common.mk

bin_PROGRAMS = kr

kr_SOURCES = kr.c
kr_CFLAGS = $(OPUS_CFLAGS)
kr_CFLAGS += -DKR_GIT_VERSION=\"$(KR_GIT_VERSION)\"
kr_LDFLAGS = ../lib/krad_radio/libkradclient.la
