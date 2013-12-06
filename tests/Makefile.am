include $(top_srcdir)/common.mk

bin_PROGRAMS = kr_cbor_test

if HAVE_WAYLAND
  bin_PROGRAMS += kr_wayland_test
endif

kr_cbor_test_SOURCES = kr_cbor_test.c
kr_cbor_test_SOURCES += ../lib/krad_cbor/krad_cbor.c
kr_cbor_test_CFLAGS =
kr_cbor_test_LDFLAGS =

kr_wayland_test_SOURCES = kr_wayland_test.c
kr_wayland_test_SOURCES += ../lib/krad_wayland/krad_wayland.c
kr_wayland_test_CFLAGS =
kr_wayland_test_LDFLAGS = $(WAYLAND_LIBS) $(XKBCOMMON_LIBS)

.PHONY: all
