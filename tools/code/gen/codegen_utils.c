#include "codegen_utils.h"

int memb_to_print_format(struct struct_memb_def *memb, char *code) {

  if (!strcmp(memb->type,"int") || !strncmp(memb->type,"int32_t",7)) {
    strncpy(code,"%d",2);
    code[2] = '\0';
    return 1;
  }

  if (!strcmp(memb->type,"uint") || !strncmp(memb->type,"uint32_t",8)) {
    strncpy(code,"%u",2);
    code[2] = '\0';
    return 1;
  }

  if (!strncmp(memb->type,"int64_t",7)) {
    strncpy(code,"%jd",3);
    code[3] = '\0';
    return 1;
  }

  if (!strncmp(memb->type,"uint64_t",8)) {
    strncpy(code,"%ju",3);
    code[3] = '\0';
    return 1;
  }

  if (!strncmp(memb->type,"float",5)) {
    strncpy(code,"%0.2f",5);
    code[6] = '\0';
    return 1;
  }

  if (!strncmp(memb->type,"char",4)) {
    if (memb->array) {
      strncpy(code,"%s",2);
      code[2] = '\0';
      return 1;
    } else  if (memb->pointer) {
        if (memb->pointer == 1) {
          strncpy(code,"%s",2);
          code[2] = '\0';
        } else {
          return 0;
        }
    } else {
      strncpy(code,"%c",2);
      code[2] = '\0';
    }
  }

  return 0;
}

static void codegen_prototype(struct struct_def *def, const char *type, 
  FILE *out) {
  fprintf(out,"int %s_to_%s(char *%s, void *st, int32_t max);\n",
    def->name,type,type);
  return;
}

static void codegen_function(struct struct_def *def, char *type, 
  gen_format gformat, FILE *out) {

  if (gformat == EBML) {
    fprintf(out,"int %s_to_%s(char *%s, void *st) {\n  int res;\n\n  res = 0;\n  uber_St uber;\n",
      def->name,type,type);
  } else {
    fprintf(out,"int %s_to_%s(char *%s, void *st, int32_t max) {\n  int res;\n\n  res = 0;\n  uber_St uber;\n",
      def->name,type,type);
  }

  if (def->istypedef) {
    fprintf(out,"  %s *actual;\n\n",def->name);
  } else {
    fprintf(out,"  struct %s *actual;\n\n",def->name);
  }

  if (gformat == EBML) {
    fprintf(out,"  if ((%s == NULL) || (st == NULL)) {\n    return -1;\n  }\n\n",type);
  } else {
    fprintf(out,"  if ((%s == NULL) || (st == NULL) || (max < 1)) {\n    return -1;\n  }\n\n",type);
  }

  if (def->istypedef) {
    fprintf(out,"  actual = (%s*)st;\n\n",def->name);
  } else {
    fprintf(out,"  actual = (struct %s*)st;\n\n",def->name);
  }

  switch (gformat) {
    case TEXT: codegen_text(def,type,out); break;
    case JSON: codegen_json(def,type,out); break;
    case EBML: codegen_ebml(def,type,out); break;
    case CBOR: break;
  }
  
  fprintf(out,"\n  return res;\n}\n\n");

  return;
}

static int codegen_array_func(struct struct_def **fdefs, int n, 
  const char *type, FILE *out) {

  int i;

  fprintf(out,"int info_pack_to_%s(char *%s, uber_St *uber, int max) {\n",
    type,type);
  fprintf(out,"  const info_pack_to_%s_func to_%s_functions[%d] = {",
    type,type,n);

  for (i = 0; i< n; i++) {
    if (i != (n-1)) {
      fprintf(out," %s_to_%s,",fdefs[i]->name,type);
    } else {
      fprintf(out," %s_to_%s",fdefs[i]->name,type);
    }

    if (!(i%2)) {
      fprintf(out,"\n  ");
    }
  }

  fprintf(out,"};\n  return to_%s_functions[uber->type-1](%s , uber->actual, max);\n}\n",
    type,type);

  return 0;
}

static int codegen_internal(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix, char *format, gen_format gformat , FILE *out) {

  int i;
  char *p;
  int n = 0;
  struct struct_def *filtered_defs[ndefs];
  char *formatc = strdup(format);
  
  p = strchr(formatc,'/');

  if (!p) {
    return 0;
  }

  p[0] = '\0';

  for (i = 0; i < ndefs; i++) {
    if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix)) {
      filtered_defs[n] = &defs[i];
      n++;
    }
  }

  if (!strncmp(&format[4],"/proto",6)) {
    for (i = 0; i < n; i++) {
      codegen_prototype(filtered_defs[i],formatc,out);
    }
  } else if (!strncmp(&format[4],"/func",5)) {
    for (i = 0; i < n; i++) {
      codegen_function(filtered_defs[i],formatc,gformat,out);
    }
  } else if (!strncmp(&format[4],"/array_func",11)) {
    codegen_array_func(filtered_defs,n,formatc,out);
  } else if (!strncmp(&format[4],"/typedef",9)) {
    fprintf(out,"int info_pack_to_%s(char *%s, uber_St *uber, int max);\n",
      formatc,formatc);
    codegen_typedef(formatc,out);
  } 

  if (formatc) {
    free(formatc);
  }

  return 0;
}

int codegen(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix, char *format, FILE *out) {

  gen_format gformat;

  gformat = 0;

  if (!strncmp(format,"ebml",4)) {
    gformat = EBML;
  } else if (!strncmp(format,"json",4)) {
    gformat = JSON;
  } else if (!strncmp(format,"text",4)) {
    gformat = TEXT;
  } else if (!strncmp(format,"cbor",4)) {
    gformat = CBOR;
  }

  if (gformat) {
    codegen_internal(defs,ndefs,prefix,suffix,format,gformat,out);
  }

  if (!strncmp(format,"sizeof",6)) {
    codegen_sizeof(defs,ndefs,prefix,suffix,out);
    return 0;
  }

  if (!strncmp(format,"jschema",7)) {
    codegen_jschema(defs,ndefs,prefix,suffix,out);
    return 0;
  }

  if (!strncmp(format,"enum_utils",10)) {
    codegen_enum_util_functions(defs,ndefs,prefix,suffix,out);
    return 0;
  }

  if (!strncmp(format,"enum",4)) {
    codegen_enum(defs,ndefs,prefix,suffix,out);
    return 0;
  }

  if (!strncmp(format,"includes",8)) {
    codegen_includes(defs,ndefs,prefix,suffix,out);
    return 0;
  }

  if (!strncmp(format,"helper_proto",12)) {
    codegen_helpers_prototype(defs,ndefs,prefix,suffix,out);
    return 0;
  }

  if (!strncmp(format,"helper",6)) {
    codegen_helper_functions(defs,ndefs,prefix,suffix,out);
    return 0;
  }

  return 1;
}
