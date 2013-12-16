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
  gen_format gformat, FILE *out) {
  if (gformat == EBML || gformat == DEBML) {
    if (gformat == EBML) {
      fprintf(out,"int %s_to_%s(kr_ebml *%s, void *st);\n",
        def->name,type,type);
    } else {
      fprintf(out,"int %s_fr_%s(kr_ebml *%s, void *st);\n",
        def->name,&type[1],&type[1]);
    }
  } else {
    fprintf(out,"int %s_to_%s(char *%s, void *st, int32_t max);\n",
      def->name,type,type);
  }
  return;
}

static void codegen_function(struct struct_def *def, char *type, 
  gen_format gformat, FILE *out) {

  if (gformat == EBML || gformat == DEBML) {
    if (gformat == EBML) {
      fprintf(out,"int %s_to_%s(kr_ebml *%s, void *st) {\n  int i;\n  int res;\n  res = 0;\n  uber_St uber;\n",
        def->name,type,type);
    } else {
      fprintf(out,"int %s_fr_%s(kr_ebml *%s, void *st) {\n  int i;\n  int res;\n  res = 0;\n  uber_St uber;\n",
        def->name,&type[1],&type[1]);
    }
  } else {
    fprintf(out,"int %s_to_%s(char *%s, void *st, int32_t max) {\n  int i;\n  int res;\n  res = 0;\n  uber_St uber;\n",
      def->name,type,type);
  }

  if (def->istypedef) {
    fprintf(out,"  %s *actual;\n\n",def->name);
  } else {
    fprintf(out,"  struct %s *actual;\n\n",def->name);
  }

  if (gformat == EBML || gformat == DEBML) {
    if (gformat == EBML) {
      fprintf(out,"  if ((%s == NULL) || (st == NULL)) {\n    return -1;\n  }\n\n",type);
    } else {
      fprintf(out,"  if ((%s == NULL) || (st == NULL)) {\n    return -1;\n  }\n\n",&type[1]);
    }
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
    case EBML: codegen_ebml(def,type,EBML_PACK,out); break;
    case DEBML: codegen_ebml(def,type,EBML_UNPACK,out); break;
    case CBOR: break;
  }
  
  fprintf(out,"\n  return res;\n}\n\n");

  return;
}

static void codegen_enum_value(struct struct_def *def, FILE *out, char *format) {
  char uppercased[strlen(def->name)+1];
  uppercase(def->name,uppercased);
  fprintf(out,"%s_%s",format,uppercased);
}

int codegen_enum(struct header_defs *hdefs, int ndefs, char *prefix,
 char *suffix, FILE *out, cgen_target_type type) {

  int i;
  int j;
  int l;
  int total;
  int n;
  char *format;
  struct header_defs *fhdefs[ndefs];

  total = 0;
  n = 0;
  fprintf(out,"typedef enum {\n");

  switch (type) {
    case TO_TEXT: format = "text"; break;
    case TO_EBML: format = "ebml"; break;
    case TO_JSON: format = "json"; break;
    case FR_EBML: format = "debml"; break;
    case HELPERS: format = "helpers"; break;
    default: format = ""; break;
  }

  char format_upp[strlen(format)];
  uppercase(format,format_upp);

  for (i=0;i<ndefs;i++) {
    if (hdefs[i].targets.ntargets) {
      for (l = 0; l < hdefs[i].targets.ntargets; l++) {
        if (hdefs[i].targets.types[l] == type) {
          fhdefs[total] = &hdefs[i];
          total++;
          break;
        }
      }
    }
  }

  for (i = 0; i < total; i++) {
    for (j=0;j<fhdefs[i]->ndefs;j++) {
      if (is_prefix(hdefs[i].defs[j].name,prefix) && is_suffix(hdefs[i].defs[j].name,suffix)) {
        fprintf(out,"  ");
        codegen_enum_value(&fhdefs[i]->defs[j],out,format_upp);
        if (i == 0 && j == 0) {
          fprintf(out," = 1");
        }
        if (j == (fhdefs[i]->ndefs-1) && i == (total - 1)) {
          fprintf(out,"\n");
        } else {
          fprintf(out,",\n");
        }
        n++;
      }
    }
  }

  fprintf(out,"} %s_enum;\n\n",format);
  fprintf(out,"enum {\n  %s_ENUM_LAST = %d\n};\n\n",format_upp,n);

  return 0;
}

int codegen_array_func(struct header_defs *hdefs, int ndefs, 
  char *prefix, char *suffix, const char *type, gen_format gformat, FILE *out) {

  int i;
  int j;
  int l;
  int n;
  int k;

  n = 0;
  k = 0;

  for (i = 0; i < ndefs; i++) {
    if (hdefs[i].targets.ntargets) {
      for (l = 0; l < hdefs[i].targets.ntargets; l++) {
        if ((gen_format)hdefs[i].targets.types[l] == gformat) {
          n += hdefs[i].ndefs;
        }
      }
    }
  }

  if (!n) {
    return 0;
  }

  struct struct_def *fdefs[n];

  for (i = 0; i < ndefs; i++) {
    for (j = 0; j < hdefs[i].ndefs; j++) {
      if (is_prefix(hdefs[i].defs[j].name,prefix) && is_suffix(hdefs[i].defs[j].name,suffix)) {
        if (hdefs[i].targets.ntargets) {
          for (l = 0; l < hdefs[i].targets.ntargets; l++) {
            if ((gen_format)hdefs[i].targets.types[l] == gformat) {
              fdefs[k] = &hdefs[i].defs[j];
              k++;
              break;
            }
          }
        }
      }
    }
  }

  if (gformat == EBML || gformat == DEBML) {
    if (gformat == EBML) {
      fprintf(out,"int info_pack_to_%s(kr_ebml *%s, uber_St *uber) {\n",
        type,type);
    } else {
      fprintf(out,"int info_unpack_fr_%s(kr_ebml *%s, uber_St *uber) {\n",
        &type[1],&type[1]);
    }
  } else {
    fprintf(out,"int info_pack_to_%s(char *%s, uber_St *uber, int max) {\n",
      type,type);
  }

  if (gformat == DEBML) {
    fprintf(out,"  const info_unpack_fr_%s_func fr_%s_functions[%d] = {",
    &type[1],&type[1],n);
  } else {
    fprintf(out,"  const info_pack_to_%s_func to_%s_functions[%d] = {",
    type,type,n);
  }

  for (i = 0; i < n; i++) {
    if (i != (n-1)) {
      if (gformat == DEBML) {
        fprintf(out," %s_fr_%s,",fdefs[i]->name,&type[1]);
      } else {
        fprintf(out," %s_to_%s,",fdefs[i]->name,type);
      }
    } else {
     if (gformat == DEBML) {
        fprintf(out," %s_fr_%s",fdefs[i]->name,&type[1]);
      } else {
        fprintf(out," %s_to_%s",fdefs[i]->name,type);
      }
    }

    if (!(i%2)) {
      fprintf(out,"\n  ");
    }
  }

  if (gformat == EBML || gformat == DEBML) {
    if (gformat == EBML) {
      fprintf(out,"};\n  return to_%s_functions[uber->type-1](%s , uber->actual);\n}\n",
        type,type);
    } else {
      fprintf(out,"};\n  return fr_%s_functions[uber->type-1](%s , uber->actual);\n}\n",
        &type[1],&type[1]);
    }
  } else {
    fprintf(out,"};\n  return to_%s_functions[uber->type-1](%s , uber->actual, max);\n}\n",
      type,type);
  }

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

  if (!strncmp(&p[1],"proto",5)) {
    for (i = 0; i < n; i++) {
      codegen_prototype(filtered_defs[i],formatc,gformat,out);
    }
  } else if (!strncmp(&p[1],"func",4)) {
    for (i = 0; i < n; i++) {
      codegen_function(filtered_defs[i],formatc,gformat,out);
    }
  } else if (!strncmp(&p[1],"array_func",10)) {
    //codegen_array_func(filtered_defs,n,formatc,gformat,out);
  } else if (!strncmp(&p[1],"typedef",8)) {
    if (gformat == EBML || gformat == DEBML) {
      if (gformat == EBML) {
        fprintf(out,"int info_pack_to_%s(kr_ebml *%s, uber_St *uber);\n",
          formatc,formatc);
      } else {
        fprintf(out,"int info_unpack_fr_%s(kr_ebml *%s, uber_St *uber);\n",
          &formatc[1],&formatc[1]);
      }
    } else {
      fprintf(out,"int info_pack_to_%s(char *%s, uber_St *uber, int max);\n",
        formatc,formatc);
    }

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
  } else if (!strncmp(format,"debml",5)) {
    gformat = DEBML;
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
    //codegen_enum(defs,ndefs,prefix,suffix,out);
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

  if (!strcmp(format,"helper")) {
    codegen_helper_functions(defs,ndefs,prefix,suffix,out);
    return 0;
  }

  return 1;
}
