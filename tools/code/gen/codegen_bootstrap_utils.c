#include "codegen_bootstrap_utils.h"

void capitalize(char *str1, char *str2) {
  uint32_t len;
  uint32_t i;

  len = strlen(str1);

  for(i=0;i<len;i++) {
    str2[i] = toupper(str1[i]);
  }

  str2[len] = '\0';

  return;
}

static void codegen_enum_value(struct struct_def *def, FILE *out) {
  char capitalized[strlen(def->name)+1];
  capitalize(def->name,capitalized);
  fprintf(out,"CGEN_%s",capitalized);
}

int codegen_enum(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix, FILE *out) {

  int i;

  fprintf(out,"typedef enum {\n");

  for (i=0;i<ndefs;i++) {
    if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix)) {
      fprintf(out,"  ");
      codegen_enum_value(&defs[i],out);

      if (i == 0) {
        fprintf(out," = 1");
      }

      if (i == (ndefs-1)) {
        fprintf(out,"\n");
      } else {
        fprintf(out,",\n");
      }
    }
  }

  fprintf(out,"} cgen_enum;\n\n");

  fprintf(out,"enum {\n  CODEGEN_ENUM_LAST = %d\n};\n",ndefs);

  return 0;
}

void codegen_typedef(char *type, FILE *out) {
  fprintf(out,"typedef int (*info_pack_to_%s_func)(char *%s, void *st, int max);\n",type,type);
}

void codegen_includes(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix, FILE *out) {

  int i;
  char *ppath;
  char *base;

  for (i=0;i<ndefs;i++) {
    if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix)) {

      base = basename(defs[i].fullpath);

      if (i > 0) {
        if (strcmp(ppath,defs[i].fullpath)) {
          fprintf(out,"#include \"%s\"\n",base);
        }
      } else {
        fprintf(out,"#include \"%s\"\n",base);
      }

      ppath = defs[i].fullpath;

    }
  }

}

static void codegen_enum_to_string(struct struct_def *def, FILE *out) {
  char cap[strlen(def->name)+1];
  capitalize(def->name,cap);
  fprintf(out,"    case CGEN_%s:\n      return \"%s\";\n",cap,def->name);
  return;
}

static void codegen_string_to_enum(struct struct_def *def, FILE *out) {
  char cap[strlen(def->name)+1];
  capitalize(def->name,cap);
  fprintf(out,"  if (!strcmp(string,\"%s\")) {\n    return CGEN_%s;\n  }\n",def->name,cap);
  return;
}

static void codegen_enum_func_proto(FILE *out) {
  fprintf(out,"cgen_enum codegen_string_to_enum(char *string);\n");
  fprintf(out,"char *codegen_enum_to_string(cgen_enum val);\n\n");
}

int codegen_enum_utils(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix, FILE *out) {

  int i;
  int n = 0;
  struct struct_def *filtered_defs[ndefs];

  for (i=0;i<ndefs;i++) {
    if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix)) {
      filtered_defs[n] = &defs[i];
      n++;
    }
  }

  fprintf(out,"#include \"bootstrapped.h\"\n#include <string.h>\n\nchar *codegen_enum_to_string(cgen_enum val) {\n  switch (val) {\n"); 

  for (i=0;i<n;i++) {
    codegen_enum_to_string(filtered_defs[i],out);
  }

  fprintf(out,"  }\n  return \"Unknown\";\n}\n\n"); 
  fprintf(out,"cgen_enum codegen_string_to_enum(char *string) {\n");

  for (i=0;i<n;i++) {
    codegen_string_to_enum(filtered_defs[i],out);
  }

  fprintf(out,"  return 0;\n}\n\n");

  return 0;
}

int codegen_bootstrap(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix, FILE *out) {
  //codegen_typedef(format,out);
  codegen_enum(defs,ndefs,prefix,suffix,out);
  codegen_enum_func_proto(out);
  return 0;
}
