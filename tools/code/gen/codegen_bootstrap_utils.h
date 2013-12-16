#include "structs_gathering.h"
void codegen_includes(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix, FILE *out);
void codegen_typedef(char *type, FILE *out);
int codegen_bootstrap(struct header_defs *hdefs, int ndefs, char *prefix,
 char *suffix, FILE *out);
 int codegen_enum_utils(struct header_defs *hdefs, int ndefs, char *prefix,
 char *suffix, FILE *out);
 void uppercase(char *str1, char *str2);
 void lowercase(char *str1, char *str2);
