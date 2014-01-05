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

static void codegen_enum_value(struct_data *def, FILE *out) {
  char uppercased[strlen(def->info.name)+1];
  uppercase(def->info.name,uppercased);
  fprintf(out,"CGEN_%s",uppercased);
}

static int codegen_enum(header_data *hdata, int n, char *prefix,
 char *suffix, FILE *out) {

  int i;
  int j;
  int total;

  total = 0;
  fprintf(out,"typedef enum {\n");

  for (i = 0; i < n; i++) {
    for (j = 0; j < hdata[i].def_count; j++) {
      if (is_prefix(hdata[i].defs[j].info.name,prefix) && is_suffix(hdata[i].defs[j].info.name,suffix)) {
        fprintf(out,"  ");
        codegen_enum_value(&hdata[i].defs[j],out);
        if (i == 0 && j == 0) {
          fprintf(out," = 1");
        }
        if (j == (hdata[i].def_count-1) && i == (n - 1)) {
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
  } else if (!strncmp(type,"dejson",6)) {
    fprintf(out,"typedef int (*info_unpack_fr_%s_func)(char *%s, void *st);\n",&type[2],&type[2]);
  } else {
    fprintf(out,"typedef int (*info_pack_to_%s_func)(char *%s, void *st, int max);\n",type,type);
  }
}

void codegen_includes(struct_data *defs, int ndefs, char *prefix,
 char *suffix, FILE *out) {

  int i;
  char *ppath;

  for (i = 0; i < ndefs; i++) {
    if (is_prefix(defs[i].info.name,prefix) && is_suffix(defs[i].info.name,suffix)) {

      if (i > 0) {
        if (strcmp(ppath,defs[i].filename)) {
          fprintf(out,"#include \"%s\"\n",defs[i].filename);
        }
      } else {
        fprintf(out,"#include \"%s\"\n",defs[i].filename);
      }

      ppath = defs[i].filename;

    }
  }

}

static void codegen_enum_to_string(struct_data *def, FILE *out) {
  char cap[strlen(def->info.name)+1];
  uppercase(def->info.name,cap);
  fprintf(out,"    case CGEN_%s:\n      return \"%s\";\n",cap,def->info.name);
  return;
}

static void codegen_string_to_enum(struct_data *def, FILE *out) {
  char cap[strlen(def->info.name)+1];
  uppercase(def->info.name,cap);
  fprintf(out,"  if (!strcmp(string,\"%s\")) {\n    return CGEN_%s;\n  }\n",def->info.name,cap);
  return;
}

static void codegen_is_union(struct_data *def, FILE *out) {
  if (def->info.type == ST_UNION) {
    fprintf(out,"  if (!strcmp(type,\"%s\")) {\n    return 1;\n  }\n",def->info.name);
  }
  return;
}

static void codegen_is_enum(struct_data *def, FILE *out) {
  if (def->info.type == ST_ENUM) {
    fprintf(out,"  if (!strcmp(type,\"%s\")) {\n    return 1;\n  }\n",def->info.name);
  }
  return;
}

static void codegen_enum_func_proto(FILE *out) {
  fprintf(out,"int codegen_is_union(char *type);\n");
  fprintf(out,"int codegen_is_enum(char *type);\n");
  fprintf(out,"cgen_enum codegen_string_to_enum(char *string);\n");
  fprintf(out,"char *codegen_enum_to_string(cgen_enum val);\n\n");
}

int codegen_enum_utils(header_data *hdata, int nn, char *prefix,
 char *suffix, FILE *out) {

  int i;
  int j;
  int n = 0;
  struct_data *filtered_defs[nn*MAX_HEADER_DEFS];

  for (i = 0; i < nn; i++) {
    for (j = 0; j < hdata[i].def_count; j++) {
      if (is_prefix(hdata[i].defs[j].info.name,prefix) && is_suffix(hdata[i].defs[j].info.name,suffix)) {
        filtered_defs[n] = &hdata[i].defs[j];
        n++;
      }
    }
  }

  fprintf(out,"#include \"bootstrapped.h\"\n#include <string.h>\n\nchar *codegen_enum_to_string(cgen_enum val) {\n  switch (val) {\n"); 

  for (i = 0; i < n; i++) {
    codegen_enum_to_string(filtered_defs[i],out);
  }

  fprintf(out,"  }\n  return \"Unknown\";\n}\n\n"); 
  fprintf(out,"cgen_enum codegen_string_to_enum(char *string) {\n");

  for (i = 0; i < n; i++) {
    codegen_string_to_enum(filtered_defs[i],out);
  }

  fprintf(out,"  return 0;\n}\n\n");


  fprintf(out,"int codegen_is_union(char *type) {\n");

  for (i = 0; i < n; i++) {
    codegen_is_union(filtered_defs[i],out);
  }

  fprintf(out,"  return 0;\n}\n\n");

  fprintf(out,"int codegen_is_enum(char *type) {\n");

  for (i = 0; i < n; i++) {
    codegen_is_enum(filtered_defs[i],out);
  }

  fprintf(out,"  return 0;\n}\n\n");

  return 0;
}

int codegen_bootstrap(header_data *hdata, int n, char *prefix,
 char *suffix, FILE *out) {
  codegen_enum(hdata,n,prefix,suffix,out);
  codegen_enum_func_proto(out);
  return 0;
}
