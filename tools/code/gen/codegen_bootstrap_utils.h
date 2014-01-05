#include "structs_gathering.h"
void codegen_includes(struct_data *defs, int n, char *prefix,
 char *suffix, FILE *out);
void codegen_typedef(char *type, FILE *out);
int codegen_bootstrap(header_data *hdata, int n, char *prefix,
 char *suffix, FILE *out);
 int codegen_enum_utils(header_data *hdata, int n, char *prefix,
 char *suffix, FILE *out);
 void uppercase(char *str1, char *str2);
 void lowercase(char *str1, char *str2);
