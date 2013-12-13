#include "codegen_bootstrap_utils.h"

void uppercase(char *str1, char *str2) {

  uint32_t len;
  uint32_t i;

  len = strlen(str1);

  for(i=0;i<len;i++) {
    str2[i] = toupper(str1[i]);
  }

  str2[len] = '\0';

  return;
}

void lowercase(char *str1, char *str2) {
  uint32_t len;
  uint32_t i;

  len = strlen(str1);

  for(i=0;i<len;i++) {
    str2[i] = tolower(str1[i]);
  }

  str2[len] = '\0';

  return;
}

static void codegen_enum_value(struct struct_def *def, FILE *out) {
  char uppercased[strlen(def->name)+1];
  uppercase(def->name,uppercased);
  fprintf(out,"CGEN_%s",uppercased);
}

int codegen_enum(struct header_defs *hdefs, int ndefs, char *prefix,
 char *suffix, FILE *out) {

  int i;
  int j;
  int total;

  total = 0;
  fprintf(out,"typedef enum {\n");

  for (i=0;i<ndefs;i++) {
    for (j=0;j<hdefs[i].ndefs;j++) {
      if (is_prefix(hdefs[i].defs[j].name,prefix) && is_suffix(hdefs[i].defs[j].name,suffix)) {
        fprintf(out,"  ");
        codegen_enum_value(&hdefs[i].defs[j],out);
        if (i == 0 && j == 0) {
          fprintf(out," = 1");
        }
        if (j == (hdefs[i].ndefs-1) && i == (ndefs - 1)) {
          fprintf(out,"\n");
        } else {
          fprintf(out,",\n");
        }
        total++;
      }
    }
  }

  fprintf(out,"} cgen_enum;\n\n");
  fprintf(out,"enum {\n  CODEGEN_ENUM_LAST = %d\n};\n",total);

  return 0;
}

void codegen_typedef(char *type, FILE *out) {
  if (!strncmp(type,"ebml",4)) {
    fprintf(out,"typedef int (*info_pack_to_%s_func)(kr_ebml *%s, void *st);\n",type,type);
  } else if (!strncmp(type,"debml",5)) {
    fprintf(out,"typedef int (*info_unpack_fr_%s_func)(kr_ebml *%s, void *st);\n",&type[1],&type[1]);
  }
  else {
    fprintf(out,"typedef int (*info_pack_to_%s_func)(char *%s, void *st, int max);\n",type,type);
  }
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
  uppercase(def->name,cap);
  fprintf(out,"    case CGEN_%s:\n      return \"%s\";\n",cap,def->name);
  return;
}

static void codegen_string_to_enum(struct struct_def *def, FILE *out) {
  char cap[strlen(def->name)+1];
  uppercase(def->name,cap);
  fprintf(out,"  if (!strcmp(string,\"%s\")) {\n    return CGEN_%s;\n  }\n",def->name,cap);
  return;
}

static void codegen_enum_func_proto(FILE *out) {
  fprintf(out,"cgen_enum codegen_string_to_enum(char *string);\n");
  fprintf(out,"char *codegen_enum_to_string(cgen_enum val);\n\n");
}

int codegen_enum_utils(struct header_defs *hdefs, int ndefs, char *prefix,
 char *suffix, FILE *out) {

  int i;
  int j;
  int n = 0;
  struct struct_def *filtered_defs[ndefs*MAX_HEADER_DEFS];

  for (i=0;i<ndefs;i++) {
    for (j=0;j<hdefs[i].ndefs;j++) {
      if (is_prefix(hdefs[i].defs[j].name,prefix) && is_suffix(hdefs[i].defs[j].name,suffix)) {
        filtered_defs[n] = &hdefs[i].defs[j];
        n++;
      }
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

int codegen_bootstrap(struct header_defs *hdefs, int ndefs, char *prefix,
 char *suffix, FILE *out) {
  //codegen_typedef(format,out);
  codegen_enum(hdefs,ndefs,prefix,suffix,out);
  codegen_enum_func_proto(out);
  return 0;
}
