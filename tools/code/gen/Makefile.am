include $(top_srcdir)/common.mk
AM_CPPFLAGS += '-I../analysis/gather_structs/'
noinst_PROGRAMS = codegen_bootstrap codegen_pre codegen_auto codegen_cfg
.PHONY: cleanall
codegen_pre$(EXEEXT): ; ./codegen_bootstrap ../../../lib "" ""
codegen_bootstrap_SOURCES = ../analysis/gather_structs/structs_gathering.c codegen_bootstrap.c codegen_bootstrap_utils.c
codegen_auto_SOURCES = ../analysis/gather_structs/structs_gathering.c codegen_utils.c \
codegen_text.c codegen_json.c codegen_dejson.c codegen_sizeof.c codegen_jschema.c codegen_ebml.c codegen_helper.c \
codegen_bootstrap_utils.c bootstrapped.c codegen_auto.c
codegen_cfg_SOURCES = ../analysis/gather_structs/structs_gathering.c codegen_cfg.c
cleanall: clean
	rm -rf bootstrapped.c bootstrapped.h codegen_auto
