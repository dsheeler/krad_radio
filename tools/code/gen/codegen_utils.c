#include "codegen_utils.h"

int memb_struct_check(member_info *memb) {
  if (memb->type == T_STRUCT) {
    return codegen_string_to_enum(memb->type_info.substruct_info.type_name);
  } else {
    return 0;
  }
}

int memb_to_print_format(member_info *memb, char *code) {

  char *type;

  type = member_type_to_str(memb->type);

  if (!strcmp(type,"int") || !strncmp(type,"int32_t",7)) {
    strncpy(code,"%d",2);
    code[2] = '\0';
    return 1;
  }

  if (!strcmp(type,"uint") || !strncmp(type,"uint32_t",8)) {
    strncpy(code,"%u",2);
    code[2] = '\0';
    return 1;
  }

  if (!strncmp(type,"int64_t",7)) {
    strncpy(code,"%jd",3);
    code[3] = '\0';
    return 1;
  }

  if (!strncmp(type,"uint64_t",8)) {
    strncpy(code,"%ju",3);
    code[3] = '\0';
    return 1;
  }

  if (!strncmp(type,"float",5) || !strncmp(type,"double",6)) {
    strncpy(code,"%0.2f",5);
    code[5] = '\0';
    return 1;
  }

  if (!strncmp(type,"char",4)) {
    if (memb->arr) {
      strncpy(code,"%s",2);
      code[2] = '\0';
      return 1;
    } else  if (memb->ptr) {
        if (memb->ptr == 1) {
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

static void codegen_validity_check_gen(gen_format gformat, char *type, FILE *out) {

  char *rtype;
  char *max_check;

  rtype = type;
  max_check = "";

  switch (gformat) {
    case TEXT: 
    case JSON: {
      max_check = " || (max < 1)";
      break;
    }
    case DEJSON: {
      rtype = &type[2]; 
      break;
    }
    case EBML: break;
    case DEBML: {
      rtype = &type[1]; 
      break;
    }
    case CBOR: break;
  }

  fprintf(out,"  if ((%s == NULL) || (st == NULL)%s) {\n    return -1;\n  }\n\n",rtype,max_check);

}

static void codegen_funname_gen(char *name, char* type, FILE *out) {

  char *rtype;
  char *dir;

  if (!strncmp(type,"de",2)) {
    dir = "fr";
    if (!strncmp(type,"dejson",6)) {
      rtype = &type[2];
    } else {
      rtype = &type[1];
    }
  } else {
    dir = "to";
    rtype = type;
  }

  fprintf(out,"%s_%s_%s",name,dir,rtype);

}

static void codegen_func_array_name_gen(char *type, int n, FILE *out) {

  char *rtype;
  char *dir;
  char *ftype;

  if (!strncmp(type,"de",2)) {
    dir = "fr";
    ftype = "unpack";
    if (!strncmp(type,"dejson",6)) {
      rtype = &type[2];
    } else {
      rtype = &type[1];
    }
  } else {
    dir = "to";
    ftype = "pack";
    rtype = type;
  }

  fprintf(out,"  const info_%s_%s_%s_func %s_%s_functions[%d] = {",ftype,dir,rtype,dir,rtype,n);
}

static void codegen_func_array_return_gen(char *type, FILE *out) {
  char *rtype;
  char *dir;
  char *thirdarg;

  thirdarg = "";

  if (!strncmp(type,"de",2)) {
    dir = "fr";
    if (!strncmp(type,"dejson",6)) {
      rtype = &type[2];
    } else {
      rtype = &type[1];
    }
  } else {
    dir = "to";
    if (strncmp(type,"ebml",4)) {
      thirdarg = ", max";
    }
    rtype = type;
  }

  fprintf(out,"};\n  return %s_%s_functions[uber->type-1](%s , uber->actual%s);\n}\n",
    dir,rtype,rtype,thirdarg);
}

static void codegen_prototype_gen(char *prefix, char *type, char *secarg,
  gen_format gformat, FILE *out) {
  char *rtype;
  char *dir = "";
  char *argtype = "";
  char *thirdarg = "";

  rtype = type;

  switch (gformat) {
    case TEXT: 
    case JSON: {
      dir = "to"; 
      argtype = "char"; 
      thirdarg = ", int32_t max"; 
      break;
    }
    case DEJSON: {
      dir = "fr"; 
      argtype = "char"; 
      rtype = &type[2]; 
      break;
    }
    case EBML: {
      dir = "to"; 
      argtype = "kr_ebml"; 
      break;
    }
    case DEBML: {
      dir = "fr"; 
      argtype = "kr_ebml"; 
      rtype = &type[1]; 
      break;
    }
    case CBOR: break;
  }

  fprintf(out,"int %s_%s_%s(%s *%s, %s%s)",
      prefix,dir,rtype,argtype,rtype,secarg,thirdarg);

  return;
}

static void codegen_prototype(struct_data *def, char *type, 
  gen_format gformat, FILE *out) {
  codegen_prototype_gen(def->info.name,type,"void *st",gformat,out);
  return;
}

static void codegen_function(struct_data *def, char *type, 
  gen_format gformat, FILE *out) {

  int i;
  char decl[512];
  int res;

  memset(decl,0,256);
  res = 0;

  for (i = 0; i < def->info.member_count; i++) {
    if (memb_struct_check(&def->info.members[i])
     || (def->info.members[i].ptr))  {
      res += sprintf(&decl[res],"  uber_St uber;\n");
      break;
    }
  }

  for (i = 0; i < def->info.member_count; i++) {
    if (memb_struct_check(&def->info.members[i]) && 
      codegen_is_union(def->info.members[i].type_info.substruct_info.type_name) && (i > 0) ) {
      res += sprintf(&decl[res],"  uber_St uber_sub;\n");
      res += sprintf(&decl[res],"  int index;\n");
      break;
    }
  }

  for (i = 0; i < def->info.member_count; i++) {
    if ( (def->info.members[i].arr || def->info.members[i].len_def[0])
     && def->info.members[i].type != T_CHAR) {
      res += sprintf(&decl[res],"  int i;\n");
      break;
    }
  }

  res += sprintf(&decl[res],"  int res;\n");

  if (gformat == DEJSON) {
    res += sprintf(&decl[res],"  jsmn_parser parser;\n  jsmntok_t tokens[%d];\n"
    "  jsmnerr_t err;\n  int ntokens;\n  int k;\n",JSON_MAX_TOKENS);
  }
  
  codegen_prototype(def,type,gformat,out);

  if (def->info.type == ST_UNION) {
    res += sprintf(&decl[res],"  uber_St *uber_actual;\n\n");
  } 

  if (def->info.is_typedef) {
    res += sprintf(&decl[res],"  %s *actual;\n\n",def->info.name);
  } else {
    res += sprintf(&decl[res],"  struct %s *actual;\n\n",def->info.name);
  }

  fprintf(out," {\n%s",decl);

  fprintf(out,"  res = 0;\n\n");

  codegen_validity_check_gen(gformat,type,out);

  if (def->info.type == ST_UNION) {
    fprintf(out,"  uber_actual = (uber_St *)st;\n\n");
    fprintf(out,"  if (uber_actual->actual == NULL) {\n    return -1;\n  }\n\n");
    if (def->info.is_typedef) {
      fprintf(out,"  actual = (%s *)uber_actual->actual;\n\n",def->info.name);
    } else {
      fprintf(out,"  actual = (struct %s *)uber_actual->actual;\n\n",def->info.name);
    }
  } else {
    if (def->info.is_typedef) {
      fprintf(out,"  actual = (%s *)st;\n\n",def->info.name);
    } else {
      fprintf(out,"  actual = (struct %s *)st;\n\n",def->info.name);
    }
  }

  switch (gformat) {
    case TEXT: codegen_text(def,type,out); break;
    case JSON: codegen_json(def,type,out); break;
    case DEJSON: codegen_dejson(def,type,out); break;
    case EBML: codegen_ebml(def,type,EBML_PACK,out); break;
    case DEBML: codegen_ebml(def,type,EBML_UNPACK,out); break;
    case CBOR: break;
  }
  
  fprintf(out,"\n  return res;\n}\n\n");

  return;
}

static void codegen_enum_value(struct_data *def, FILE *out, char *format) {
  char uppercased[strlen(def->info.name)+1];
  uppercase(def->info.name,uppercased);
  fprintf(out,"%s_%s",format,uppercased);
}

int codegen_enum(header_data *hdata, int nn, char *prefix,
 char *suffix, FILE *out, cgen_target_type type) {

  int i;
  int j;
  int l;
  int total;
  int n;
  char *format;
  header_data *fhdata[nn];

  total = 0;
  n = 0;
  fprintf(out,"typedef enum {\n");

  switch (type) {
    case TO_TEXT: format = "text"; break;
    case TO_EBML: format = "ebml"; break;
    case TO_JSON: format = "json"; break;
    case FR_JSON: format = "dejson"; break;
    case FR_EBML: format = "debml"; break;
    case HELPERS: format = "helpers"; break;
    default: format = ""; break;
  }

  char format_upp[strlen(format)];
  uppercase(format,format_upp);

  for (i = 0; i < nn; i++) {
    if (hdata[i].target_count) {
      for (l = 0; l < hdata[i].target_count; l++) {
        if (hdata[i].targets[l].type == type) {
          fhdata[total] = &hdata[i];
          total++;
          break;
        }
      }
    }
  }

  for (i = 0; i < total; i++) {
    for (j = 0; j < fhdata[i]->def_count; j++) {
      if (is_prefix(hdata[i].defs[j].info.name,prefix) && is_suffix(hdata[i].defs[j].info.name,suffix)) {
        fprintf(out,"  ");
        codegen_enum_value(&fhdata[i]->defs[j],out,format_upp);
        if (i == 0 && j == 0) {
          fprintf(out," = 1");
        }
        if (j == (fhdata[i]->def_count-1) && i == (total - 1)) {
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

int codegen_array_func(header_data *hdata, int nn, 
  char *prefix, char *suffix, char *type, gen_format gformat, FILE *out) {

  int i;
  int j;
  int l;
  int n;
  int k;

  n = 0;
  k = 0;

  for (i = 0; i < nn; i++) {
    for (l = 0; l < hdata[i].target_count; l++) {
      if ((gen_format)hdata[i].targets[l].type == gformat) {
        n += hdata[i].def_count;
      }
    }
  }

  if (!n) {
    return 0;
  }

  struct_data *fdefs[n];

  for (i = 0; i < nn; i++) {
    for (j = 0; j < hdata[i].def_count; j++) {
      if (is_prefix(hdata[i].defs[j].info.name,prefix) && is_suffix(hdata[i].defs[j].info.name,suffix)) {
        for (l = 0; l < hdata[i].target_count; l++) {
          if ((gen_format)hdata[i].targets[l].type == gformat) {
            fdefs[k] = &hdata[i].defs[j];
            k++;
            break;
          }
        }
      }
    }
  }

  if (!strncmp(type,"de",2)) {
    codegen_prototype_gen("info_unpack",type,"uber_St *uber",gformat,out);
  } else {
    codegen_prototype_gen("info_pack",type,"uber_St *uber",gformat,out);
  }

  fprintf(out," {\n");

  codegen_func_array_name_gen(type,n,out);

  for (i = 0; i < k; i++) {
    codegen_funname_gen(fdefs[i]->info.name,type,out);
    if (i != (n-1)) {
      fprintf(out,",");
    } 
    if (!(i%2)) {
      fprintf(out,"\n  ");
    }
  }

  codegen_func_array_return_gen(type,out);

  return 0;
}

static int codegen_internal(struct_data *defs, int nn, char *prefix,
 char *suffix, char *format, gen_format gformat , FILE *out) {

  int i;
  char *p;
  int n = 0;
  struct_data *filtered_defs[nn];
  char *formatc = strdup(format);
  
  p = strchr(formatc,'/');

  if (!p) {
    return 0;
  }

  p[0] = '\0';

  for (i = 0; i < nn; i++) {
    if (is_prefix(defs[i].info.name,prefix) && is_suffix(defs[i].info.name,suffix)) {
      filtered_defs[n] = &defs[i];
      n++;
    }
  }

  if (!strncmp(&p[1],"proto",5)) {
    for (i = 0; i < n; i++) {
      codegen_prototype(filtered_defs[i],formatc,gformat,out);
      fprintf(out,";\n");
    }
  } else if (!strncmp(&p[1],"func",4)) {
    for (i = 0; i < n; i++) {
      codegen_function(filtered_defs[i],formatc,gformat,out);
    }
  } else if (!strncmp(&p[1],"array_func",10)) {
    //codegen_array_func(filtered_defs,n,formatc,gformat,out);
  } else if (!strncmp(&p[1],"typedef",8)) {

    if (!strncmp(formatc,"de",2)) {
      codegen_prototype_gen("info_unpack",formatc,"uber_St *uber",gformat,out);
    } else {
      codegen_prototype_gen("info_pack",formatc,"uber_St *uber",gformat,out);
    }
    fprintf(out,";\n");
    codegen_typedef(formatc,out);
  } 

  if (formatc) {
    free(formatc);
  }

  return 0;
}

int codegen(struct_data *defs, int n, char *prefix,
 char *suffix, char *format, FILE *out) {

  gen_format gformat;

  gformat = 0;

  if (!strncmp(format,"ebml",4)) {
    gformat = EBML;
  } else if (!strncmp(format,"debml",5)) {
    gformat = DEBML;
  } else if (!strncmp(format,"json",4)) {
    gformat = JSON;
  } else if (!strncmp(format,"dejson",6)) {
    gformat = DEJSON;
  } else if (!strncmp(format,"text",4)) {
    gformat = TEXT;
  } else if (!strncmp(format,"cbor",4)) {
    gformat = CBOR;
  }

  if (gformat) {
    codegen_internal(defs,n,prefix,suffix,format,gformat,out);
  }

  if (!strncmp(format,"sizeof",6)) {
    codegen_sizeof(defs,n,prefix,suffix,out);
    return 0;
  }

  if (!strncmp(format,"jschema",7)) {
    codegen_jschema(defs,n,prefix,suffix,out);
    return 0;
  }

  if (!strncmp(format,"enum_utils",10)) {
    codegen_enum_util_functions(defs,n,prefix,suffix,out);
    return 0;
  }

  if (!strncmp(format,"enum",4)) {
    //codegen_enum(defs,n,prefix,suffix,out);
    return 0;
  }

  if (!strncmp(format,"includes",8)) {
    codegen_includes(defs,n,prefix,suffix,out);
    return 0;
  }

  if (!strncmp(format,"helper_proto",12)) {
    codegen_helpers_prototype(defs,n,prefix,suffix,out);
    return 0;
  }

  if (!strcmp(format,"helper")) {
    codegen_helper_functions(defs,n,prefix,suffix,out);
    return 0;
  }

  return 1;
}
