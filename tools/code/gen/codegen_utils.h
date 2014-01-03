#include "codegen_bootstrap_utils.h"
#include "bootstrapped.h"
#define JSON_MAX_TOKENS 512

typedef enum {
  TEXT = 1,
  JSON,
  DEJSON,
  EBML,
  DEBML,
  CBOR
} gen_format;

typedef enum {
  EBML_UNPACK = 0,
  EBML_PACK = 1
} ebml_ftype;

int memb_to_print_format(member_info *memb, char *code);
int memb_struct_check(member_info *memb);
int codegen_array_func(header_data *hdefs, int n, 
  char *prefix, char *suffix, char *type, gen_format gformat, FILE *out);
void codegen_json(struct_data *def, char *type, FILE *out);
void codegen_dejson(struct_data *def, char *type, FILE *out);
void codegen_text(struct_data *def, char *type, FILE *out);
void codegen_ebml(struct_data *def, char *type, ebml_ftype ebml_fun_type, FILE *out);
int codegen(struct_data *defs, int n, char *prefix,
 char *suffix,char *format, FILE *out);
int codegen_helper_functions(struct_data *defs, int n, char *prefix,
 char *suffix, FILE *out);
void codegen_helpers_prototypes(struct_data *defs, int n, char *prefix,
 char *suffix, FILE *out);
int codegen_enum_util_functions(struct_data *defs, int n, 
  char *prefix, char *suffix, FILE *out);
int codegen_sizeof(struct_data *defs, int n, 
  char *prefix, char *suffix, FILE *out);
int codegen_jschema(struct_data *defs, int n, 
  char *prefix, char *suffix, FILE *out);
int codegen_enum(header_data *hdefs, int n, char *prefix,
 char *suffix, FILE *out, cgen_target_type type);
