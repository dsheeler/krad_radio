include $(top_srcdir)/common.mk

codegen_bootstrap_CPPFLAGS = '-I../analysis/gather_structs/'
codegen_bootstrap_CPPFLAGS += '-I../'
codegen_cfg_CPPFLAGS = ${codegen_bootstrap_CPPFLAGS}
codegen_auto_CPPFLAGS = ${codegen_bootstrap_CPPFLAGS}

noinst_PROGRAMS = codegen_bootstrap codegen_pre codegen_cfg codegen_auto

.PHONY: cleanall

codegen_pre$(EXEEXT): ; ./codegen_bootstrap ../../../lib "" ""

codegen_bootstrap_SOURCES = ../analysis/gather_structs/structs_gathering.c \
codegen_bootstrap.c \
codegen_bootstrap_utils.c

codegen_auto_SOURCES = ../analysis/gather_structs/structs_gathering.c \
codegen_utils.c \
formats/codegen_text.c \
formats/codegen_json.c \
formats/codegen_dejson.c \
formats/codegen_ebml.c \
formats/codegen_helper.c \
formats/codegen_sizeof.c \
formats/codegen_jschema.c \
codegen_bootstrap_utils.c \
bootstrapped.c \
codegen_auto.c

codegen_cfg_SOURCES = ../analysis/gather_structs/structs_gathering.c \
codegen_cfg.c

cleanall: clean
	rm -rf bootstrapped.c bootstrapped.h codegen_auto
