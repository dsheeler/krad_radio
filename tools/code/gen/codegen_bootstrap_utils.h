#include "structs_gathering.h"

int codegen_enum(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix, FILE *out);
void codegen_includes(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix, FILE *out);
void codegen_typedef(char *type, FILE *out);
int codegen_bootstrap(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix, FILE *out);
 int codegen_enum_utils(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix, FILE *out);
 void capitalize(char *str1, char *str2);
