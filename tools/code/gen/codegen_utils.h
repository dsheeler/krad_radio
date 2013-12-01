#include "codegen_bootstrap_utils.h"
#include "bootstrapped.h"

typedef enum {
  TEXT = 1,
  JSON = 2,
  EBML = 3,
  CBOR = 4
} gen_format;

int memb_to_print_format(struct struct_memb_def *memb, char *code);
void codegen_union_content_from_type(struct struct_def *def, 
  char *name, char *format, FILE *out);
void codegen_json(struct struct_def *def, char *type, FILE *out);
void codegen_text(struct struct_def *def, char *type, FILE *out);
void codegen_ebml(struct struct_def *def, char *type, FILE *out);
int codegen(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix,char *format, FILE *out);
int codegen_helper_functions(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix, FILE *out);
void codegen_helpers_prototype(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix, FILE *out);
int codegen_enum_util_functions(struct struct_def *defs, int ndefs, 
  char *prefix, char *suffix, FILE *out);
int codegen_sizeof(struct struct_def *defs, int ndefs, 
  char *prefix, char *suffix, FILE *out);
int codegen_jschema(struct struct_def *defs, int ndefs, 
  char *prefix, char *suffix, FILE *out);

