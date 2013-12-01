include $(top_srcdir)/common.mk

AM_CPPFLAGS = '-I../analysis/gather_structs/'
bin_PROGRAMS = codegen_bootstrap codegen_pre codegen 
.PHONY: cleanall
codegen_pre$(EXEEXT): ; ./codegen_bootstrap ../../../lib/krad_compositor "" ""
codegen_bootstrap_SOURCES = ../analysis/gather_structs/structs_gathering.c codegen_bootstrap.c codegen_bootstrap_utils.c
codegen_SOURCES = ../analysis/gather_structs/structs_gathering.c codegen_utils.c \
codegen_text.c codegen_json.c codegen_sizeof.c codegen_jschema.c codegen_ebml.c codegen_helper.c \
codegen_bootstrap_utils.c bootstrapped.c codegen.c
cleanall: clean
	rm -rf bootstrapped.c bootstrapped.h out.c out.h codegen main_test 


