include $(top_srcdir)/common.mk

bin_PROGRAMS = kr_cbor_test kr_pool_test

if HAVE_WAYLAND
  bin_PROGRAMS += kr_wayland_test
endif

if LINUX
  bin_PROGRAMS += kr_v4l2_test
endif

kr_cbor_test_SOURCES = kr_cbor_test.c
kr_cbor_test_SOURCES += ../lib/krad_cbor/krad_cbor.c
kr_cbor_test_CFLAGS =
kr_cbor_test_LDFLAGS =

kr_pool_test_SOURCES = kr_pool_test.c
kr_pool_test_SOURCES += ../lib/krad_mem/krad_pool.c ../lib/krad_system/krad_system.c
kr_pool_test_CFLAGS =
kr_pool_test_LDFLAGS =

kr_v4l2_test_SOURCES = kr_v4l2_test.c
kr_v4l2_test_SOURCES += ../lib/krad_v4l2/krad_v4l2.c ../lib/krad_system/krad_system.c
kr_v4l2_test_CFLAGS = -pthread
kr_v4l2_test_LDFLAGS = -lm

kr_wayland_test_SOURCES = kr_wayland_test.c
kr_wayland_test_SOURCES += ../lib/krad_wayland/krad_wayland.c
kr_wayland_test_CFLAGS =
kr_wayland_test_LDFLAGS = $(WAYLAND_LIBS) $(XKBCOMMON_LIBS)

.PHONY: all
