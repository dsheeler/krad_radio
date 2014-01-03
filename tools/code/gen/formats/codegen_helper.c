#include "codegen_utils.h"

static void codegen_helpers_prototype(struct_data *sdata, char *helper_type, FILE *out) {
  if (sdata->info.type == ST_UNION) {
    fprintf(out,"int %s_%s(void *st, int idx)",
      sdata->info.name,helper_type);
  } else {
    fprintf(out,"int %s_%s(void *st)",
      sdata->info.name,helper_type);
  }
}

static void codegen_helpers_union_content_from_type(struct_data *def, 
  char *fun_name, FILE *out) {
  int i;

  for (i = 0; i < def->info.member_count; i++) {
    if (memb_struct_check(&def->info.members[i])) {

      fprintf(out,"    case %d: {\n",i);
      fprintf(out,"      %s_%s(&actual->%s);\n",
        def->info.members[i].type_info.substruct_info.type_name,
        fun_name,def->info.members[i].name);
      fprintf(out,"      break;\n    }\n");
    }
    /* TODO: handle unions of primitives (ints,floats,strings...) */
  }
        
  return;
}

static void codegen_helper_init_func(struct_data *def, FILE *out,
 struct_data *defs, int ndefs) {

  int i;
  char cmp[sizeof(member_type_info)];
  member_info *memb;
  char arr[16];
  char indent[8];

  memset(cmp,0,sizeof(member_type_info));

  codegen_helpers_prototype(def,"init",out);

  fprintf(out," {\n");

  if (def->info.is_typedef) {      
    fprintf(out,"  %s *actual;\n\n",def->info.name);
  } else {
    fprintf(out,"  struct %s *actual;\n\n",def->info.name);
  }

  for (i = 0; i < def->info.member_count; i++) {
    if (def->info.members[i].arr || def->info.members[i].len_def[0]) {
      fprintf(out,"  int i;\n\n");
      break;
    }
  }

  fprintf(out,"  if (st == NULL) {\n    return -1;\n  }\n\n");

  if (def->info.is_typedef) {
    fprintf(out,"  actual = (%s *)st;\n",def->info.name);
    fprintf(out,"  memset(st, 0, sizeof(%s));\n",def->info.name);
  } else {
    fprintf(out,"  actual = (struct %s *)st;\n",def->info.name);
    fprintf(out,"  memset(st, 0, sizeof(struct %s));\n",def->info.name);
  }

  if (def->info.type == ST_UNION) {
    fprintf(out,"  switch (idx) {\n");
    codegen_helpers_union_content_from_type(def,"init",out);
    fprintf(out,"  }\n\n");
    fprintf(out,"\n  return -1;\n}\n\n");
    return;
  }

  for (i = 0; i < def->info.member_count; i++) {

    memb = &def->info.members[i];

    if (memb->arr || memb->len_def[0]) {
      snprintf(arr,sizeof(arr),"[i]");
      snprintf(indent,sizeof(indent),"  ");
    } else {
      arr[0] = '\0';
      indent[0] = '\0';
    }

    if (memb->arr) {
      fprintf(out,"  for (i = 0; i < %d; i++) {\n",memb->arr);
    }
    else if (memb->len_def[0]) {
      fprintf(out,"  for (i = 0; i < %s; i++) {\n",memb->len_def);
    }

    if (memcmp(&memb->type_info,cmp,sizeof(member_type_info))) {
      
      switch (def->info.members[i].type) {
        case T_CHAR: {
          if (memb->arr || (memb->ptr == 1)) {
        // TO-DO 
          }
          break;
        }
        case T_INT32: {
          fprintf(out,"%s  actual->%s%s = %d;\n",
            indent,memb->name,arr,memb->type_info.int32_info.init);
          break;
        }
        case T_INT64: {
          fprintf(out,"%s  actual->%s%s = %" PRId64 ";\n",
            indent,memb->name,arr,memb->type_info.int64_info.init);
          break;
        }
        case T_UINT32: {
          fprintf(out,"%s  actual->%s%s = %u;\n",
            indent,memb->name,arr,memb->type_info.uint32_info.init);
          break;
        }
        case T_UINT64: {
          fprintf(out,"%s  actual->%s%s = %" PRIu64 ";\n",
            indent,memb->name,arr,memb->type_info.uint64_info.init);
          break;
        }
        case T_FLOAT: {
          fprintf(out,"%s  actual->%s%s = %0.2f;\n",
            indent,memb->name,arr,memb->type_info.float_info.init);
          break;
        }
        case T_DOUBLE: {
          fprintf(out,"%s  actual->%s%s = %0.2f;\n",
            indent,memb->name,arr,memb->type_info.double_info.init);
          break;
        }
        default : break;
      }

    }

    if (memb_struct_check(memb) && codegen_is_union(memb->type_info.substruct_info.type_name)) {
      if ((i > 0) && memb_struct_check(&def->info.members[i-1])) {
        fprintf(out,"  %s%s_init(&actual->%s%s,%s_to_index(actual->%s));\n",
          indent,memb->type_info.substruct_info.type_name,memb->name,arr,
            def->info.members[i-1].type_info.substruct_info.type_name,def->info.members[i-1].name);
      }
    } else {
      if (memb_struct_check(memb)) {
        if (!codegen_is_enum(memb->type_info.substruct_info.type_name)) {
          fprintf(out,"  %s%s_init(&actual->%s%s);\n",
            indent,memb->type_info.substruct_info.type_name,memb->name,arr);
        }
      }
    }

    if (memb->arr || memb->len_def[0]) {
      fprintf(out,"  }\n");
    } 

  }

  fprintf(out,"\n  return 0;\n}\n\n");

  return;

}

static void codegen_helper_random_func(struct_data *def, FILE *out,
 struct_data *defs, int ndefs) {

  int i;
  member_info *memb;
  char cmp[sizeof(member_type_info)];
  char arr[16];
  char indent[8];

  memset(cmp,0,sizeof(member_type_info));

  codegen_helpers_prototype(def,"random",out);

  fprintf(out," {\n");

  if (def->info.is_typedef) {      
    fprintf(out,"  %s *actual;\n\n",def->info.name);
  } else {
    fprintf(out,"  struct %s *actual;\n\n",def->info.name);
  }

  for (i = 0; i < def->info.member_count; i++) {
    if (def->info.members[i].arr || def->info.members[i].len_def[0]) {
      fprintf(out,"  int i;\n");
      break;
    }
  }

  for (i = 0; i < def->info.member_count; i++) {
    memb = &def->info.members[i];
    if (memcmp(&memb->type_info,cmp,sizeof(member_type_info))) {
      if (memb->type != T_STRUCT) {
        fprintf(out,"  struct timeval tv;\n");
        fprintf(out,"  double scale;\n\n");
        fprintf(out,"  gettimeofday(&tv, NULL);\n  srand(tv.tv_sec + tv.tv_usec * 1000000ul);\n\n");
        break;
      }
    }
  }

  fprintf(out,"  if (st == NULL) {\n    return -1;\n  }\n\n");

  if (def->info.is_typedef) {
    fprintf(out,"  actual = (%s *)st;\n",def->info.name);
    fprintf(out,"  memset(st, 0, sizeof(%s));\n",def->info.name);
  } else {
    fprintf(out,"  actual = (struct %s *)st;\n",def->info.name);
    fprintf(out,"  memset(st, 0, sizeof(struct %s));\n",def->info.name);
  }

  if (def->info.type == ST_UNION) {
    fprintf(out,"  switch (idx) {\n");
    codegen_helpers_union_content_from_type(def,"random",out);
    fprintf(out,"  }\n\n");
    fprintf(out,"\n  return -1;\n}\n\n");
    return;
  }
  
  for (i = 0; i < def->info.member_count; i++) {

    memb = &def->info.members[i];

    if (memb->arr || memb->len_def[0]) {
      snprintf(arr,sizeof(arr),"[i]");
      snprintf(indent,sizeof(indent),"  ");
    } else {
      arr[0] = '\0';
      indent[0] = '\0';
    }

    if (memb->arr) {
      fprintf(out,"  for (i = 0; i < %d; i++) {\n",memb->arr);
    }
    else if (memb->len_def[0]) {
      fprintf(out,"  for (i = 0; i < %s; i++) {\n",memb->len_def);
    }

    if (memcmp(&memb->type_info,cmp,sizeof(member_type_info))) {

      switch (def->info.members[i].type) {
        case T_CHAR: {
          if (memb->arr) {
            fprintf(out,"    scale = (double)%d / RAND_MAX;\n",abs(97-122)); /* downcased ascii letters only */
            fprintf(out,"    actual->%s[i] = %d + floor(rand() * scale);\n",memb->name,97);
            fprintf(out,"    if (i == %d) {\n      actual->%s[%d] = '\\0';\n    }\n",memb->type_info.char_info.max,memb->name,memb->type_info.char_info.max);
          }
          break;
        }
        case T_INT32: {
          fprintf(out,"%s  scale = (double)%d / RAND_MAX;\n",indent,
            abs(memb->type_info.int32_info.min-memb->type_info.int32_info.max));
          fprintf(out,"%s  actual->%s%s = %d + floor(rand() * scale);\n",
            indent,memb->name,arr,memb->type_info.int32_info.min);
          break;
        }
        case T_INT64: {
          fprintf(out,"%s  scale = (double)%" PRId64 " / RAND_MAX;\n",indent,
            labs(memb->type_info.int64_info.min-memb->type_info.int64_info.max));
          fprintf(out,"%s  actual->%s%s = %" PRId64 " + floor(rand() * scale);\n",
            indent,memb->name,arr,memb->type_info.int64_info.min);
          break;
        }
        case T_UINT32: {
          fprintf(out,"%s  scale = (double)%u / RAND_MAX;\n",indent,
            abs(memb->type_info.uint32_info.min-memb->type_info.uint32_info.max));
          fprintf(out,"%s  actual->%s%s = %u + floor(rand() * scale);\n",
            indent,memb->name,arr,memb->type_info.uint32_info.min);
          break;
        }
        case T_UINT64: {
          fprintf(out,"%s  scale = (double)%" PRIu64 " / RAND_MAX;\n",indent,
            labs(memb->type_info.uint64_info.min-memb->type_info.uint64_info.max));
          fprintf(out,"%s  actual->%s%s = %" PRIu64 " + floor(rand() * scale);\n",
            indent,memb->name,arr,memb->type_info.uint64_info.min);
          break;
        }
        case T_FLOAT: {
          fprintf(out,"%s  scale = (double)%0.2f / RAND_MAX;\n",indent,
            fabs(memb->type_info.float_info.min-memb->type_info.float_info.max));
          fprintf(out,"%s  actual->%s%s = %0.2f + floor(rand() * scale);\n",
            indent,memb->name,arr,memb->type_info.float_info.min);
          break;
        }
        case T_DOUBLE: {
          fprintf(out,"%s  scale = (double)%0.2f / RAND_MAX;\n",indent,
            fabs(memb->type_info.double_info.min-memb->type_info.double_info.max));
          fprintf(out,"%s  actual->%s%s = %0.2f + floor(rand() * scale);\n",
            indent,memb->name,arr,memb->type_info.double_info.min);
          break;
        }
        default : break;
      }

    }

    if (memb_struct_check(memb) && codegen_is_union(memb->type_info.substruct_info.type_name)) {
      if ((i > 0) && memb_struct_check(&def->info.members[i-1])) {
        fprintf(out,"  %s_random(&actual->%s,%s_to_index(actual->%s));\n",
          memb->type_info.substruct_info.type_name,memb->name,
          def->info.members[i-1].type_info.substruct_info.type_name,def->info.members[i-1].name);
      }
    } else {
      if (memb_struct_check(memb)) {
        if (!codegen_is_enum(memb->type_info.substruct_info.type_name)) {
          fprintf(out,"%s  %s_random(&actual->%s%s);\n",
            indent,memb->type_info.substruct_info.type_name,memb->name,arr);
        }
      }
    }

    if (memb->arr || memb->len_def[0]) {
      fprintf(out,"  }\n");
    } 

  }

  fprintf(out,"\n  return 0;\n}\n\n");

  return;

}

static void codegen_helper_valid_func(struct_data *def, FILE *out,
 struct_data *defs, int ndefs) {

  int i;
  member_info *memb;
  char cmp[sizeof(member_type_info)];
  char arr[16];
  char indent[8];
  
  memset(cmp,0,sizeof(member_type_info));

  codegen_helpers_prototype(def,"valid",out);

  fprintf(out," {\n");

  if (def->info.is_typedef) {      
    fprintf(out,"  %s *actual;\n\n",def->info.name);
  } else {
    fprintf(out,"  struct %s *actual;\n\n",def->info.name);
  }

  for (i = 0; i < def->info.member_count; i++) {
    if (def->info.members[i].arr || def->info.members[i].len_def[0]) {
      fprintf(out,"  int i;\n\n");
      break;
    }
  }

  fprintf(out,"  if (st == NULL) {\n    return -1;\n  }\n\n");

  if (def->info.is_typedef) {
    fprintf(out,"  actual = (%s *)st;\n",def->info.name);
  } else {
    fprintf(out,"  actual = (struct %s *)st;\n",def->info.name);
  }

  if (def->info.type == ST_UNION) {
    fprintf(out,"  switch (idx) {\n");
    codegen_helpers_union_content_from_type(def,"valid",out);
    fprintf(out,"  }\n\n");
    fprintf(out,"\n  return -1;\n}\n\n");
    return;
  }
  
  for (i = 0; i < def->info.member_count; i++) {

    memb = &def->info.members[i];

    if (memb->arr || memb->len_def[0]) {
      snprintf(arr,sizeof(arr),"[i]");
      snprintf(indent,sizeof(indent),"  ");
    } else {
      arr[0] = '\0';
      indent[0] = '\0';
    }

    if (memb->type == T_CHAR) {
      if (memb->arr && memb->type_info.char_info.notnull) {
        fprintf(out,"  if (!actual->%s[0]) {\n    return %d;\n  }\n\n",memb->name,(i + 2) * -1);
      }
    }

    if (memb->arr) {
      fprintf(out,"  for (i = 0; i < %d; i++) {\n",memb->arr);
    }
    else if (memb->len_def[0]) {
      fprintf(out,"  for (i = 0; i < %s; i++) {\n",memb->len_def);
    }

    if (memcmp(&memb->type_info,cmp,sizeof(member_type_info))) {

      switch (def->info.members[i].type) {
        case T_CHAR: {
          if (memb->arr) {
            fprintf(out,"    if (!actual->%s%s) {\n      break;\n    }\n    if (i == %d && actual->%s%s) {\n      return %d;\n    }\n",
              memb->name,arr,memb->type_info.char_info.max,memb->name,arr,(i + 2) * -1);
          }
          break;
        }
        case T_INT32: {
          fprintf(out,"%s  if ( (actual->%s%s < %d) || (actual->%s%s > %d) ) {\n%s    return %d;\n  %s}\n\n",
            indent,memb->name,arr,memb->type_info.int32_info.min,
            memb->name,arr,memb->type_info.int32_info.max,indent,(i + 2) * -1,indent);
          break;
        }
        case T_INT64: {
          fprintf(out,"%s  if ( (actual->%s%s < %" PRId64 ") || (actual->%s%s > %" PRId64 ") ) {\n%s    return %d;\n  %s}\n\n",
            indent,memb->name,arr,memb->type_info.int64_info.min,
            memb->name,arr,memb->type_info.int64_info.max,indent,(i + 2) * -1,indent);
          break;
        }
        case T_UINT32: {
          fprintf(out,"%s  if ( (actual->%s%s < %u) || (actual->%s%s > %u) ) {\n%s    return %d;\n  %s}\n\n",
            indent,memb->name,arr,memb->type_info.uint32_info.min,
            memb->name,arr,memb->type_info.uint32_info.max,indent,(i + 2) * -1,indent);
          break;
        }
        case T_UINT64: {
          fprintf(out,"%s  if ( (actual->%s%s < %" PRIu64 ") || (actual->%s%s > %" PRIu64 ") ) {\n%s    return %d;\n  %s}\n\n",
            indent,memb->name,arr,memb->type_info.uint64_info.min,
            memb->name,arr,memb->type_info.uint64_info.max,indent,(i + 2) * -1,indent);
          break;
        }
        case T_FLOAT: {
         fprintf(out,"%s  if ( (actual->%s%s < %0.2f) || (actual->%s%s > %0.2f) ) {\n%s    return %d;\n  %s}\n\n",
            indent,memb->name,arr,memb->type_info.float_info.min,
            memb->name,arr,memb->type_info.float_info.max,indent,(i + 2) * -1,indent);
         break;
       }
       case T_DOUBLE: {
         fprintf(out,"%s  if ( (actual->%s%s < %0.2f) || (actual->%s%s > %0.2f) ) {\n%s    return %d;\n  %s}\n\n",
            indent,memb->name,arr,memb->type_info.double_info.min,
            memb->name,arr,memb->type_info.double_info.max,indent,(i + 2) * -1,indent);
         break;
       }
       default : break;
     }

    }

    if (memb_struct_check(memb) && codegen_is_union(memb->type_info.substruct_info.type_name)) {
      if ((i > 0) && memb_struct_check(&def->info.members[i-1])) {
        fprintf(out,"  %s_valid(&actual->%s,%s_to_index(actual->%s));\n",
          memb->type_info.substruct_info.type_name,memb->name,
          def->info.members[i-1].type_info.substruct_info.type_name,def->info.members[i-1].name);
      }
    } else {
      if (memb_struct_check(memb)) {
        if (!codegen_is_enum(memb->type_info.substruct_info.type_name)) {
          fprintf(out,"%s  %s_valid(&actual->%s%s);\n",
            indent,memb->type_info.substruct_info.type_name,memb->name,arr);
        }
      }
    }

    if (memb->arr || memb->len_def[0]) {
      fprintf(out,"  }\n");
    } 
    
  }

  fprintf(out,"\n  return 0;\n}\n\n");

  return;

}

static void codegen_enum_protos(struct_data *defs, int ndefs, 
  char *prefix, char *suffix, FILE *out) {
  int i;
  int j;
  int l;
  int n;
  struct_data *filtered_defs[ndefs];

  for (j = n = 0; j < ndefs; j++) {
    if (defs[j].info.type == ST_ENUM) {
      for (i = 0; i < ndefs; i++) {
        if (is_prefix(defs[i].info.name,prefix) && is_suffix(defs[i].info.name,suffix)) {
          for (l = 0; l < defs[i].info.member_count; l++) {
            if (!strcmp(defs[i].info.members[l].type_info.substruct_info.type_name,defs[j].info.name)) {
              filtered_defs[n] = &defs[j];
              n++;
            }
          }
        }
      }
    }
  }

  for (i = 0; i < n; i++) {
    fprintf(out,"int %s_to_index(int val);\n",filtered_defs[i]->info.name);
    fprintf(out,"int kr_strto_%s(char *string);\n",filtered_defs[i]->info.name);
    fprintf(out,"char *kr_strfr_%s(int val);\n",filtered_defs[i]->info.name);
  }

  return;
}

void codegen_helpers_prototypes(struct_data *defs, int ndefs, char *prefix,
 char *suffix, FILE *out) {

  int i;
  int n;
  struct_data *filtered_defs[ndefs];

  for (i = n = 0; i < ndefs; i++) {
    if (is_prefix(defs[i].info.name,prefix) 
      && is_suffix(defs[i].info.name,suffix) 
      && defs[i].info.type != ST_ENUM) {
      filtered_defs[n] = &defs[i];
      n++;
    }
  }

  for (i = 0; i < n; i++) {
    codegen_helpers_prototype(filtered_defs[i],"init",out);
    fprintf(out,";\n");
    codegen_helpers_prototype(filtered_defs[i],"valid",out);
    fprintf(out,";\n");
    codegen_helpers_prototype(filtered_defs[i],"random",out);
    fprintf(out,";\n");
  }

  codegen_enum_protos(defs,ndefs,prefix,suffix,out);

  return;
}

static void codegen_enum_to_index(struct_data *def, FILE *out) {
  int i;

  for (i = 0; i < def->info.member_count; i++) {
    fprintf(out,"    case %s:\n      return %d;\n",def->info.members[i].name,i);
  }

  return;
}

static void codegen_strfr_enum(struct_data *def, FILE *out) {
  int i;

  for (i = 0; i < def->info.member_count; i++) {
    char lowercased[strlen(def->info.members[i].name)+1];
    lowercase(def->info.members[i].name,lowercased);
    fprintf(out,"    case %s:\n      return \"%s\";\n",def->info.members[i].name,lowercased);
  }

  return;
}

static void codegen_strto_enum(struct_data *def, FILE *out) {
  int i;

  for (i = 0; i < def->info.member_count; i++) {
    char lowercased[strlen(def->info.members[i].name)+1];
    lowercase(def->info.members[i].name,lowercased);
    fprintf(out,"  if (!strcmp(string,\"%s\")) {\n    return %s;\n  }\n",lowercased,def->info.members[i].name);
  }

  return;
}

int codegen_enum_util_functions(struct_data *defs, int ndefs, 
  char *prefix, char *suffix, FILE *out) {

  int i;
  int j;
  int l;
  int k;
  int n;
  struct_data *filtered_defs[ndefs];
  char *enums[ndefs];


 for (i = l = 0; i < ndefs; i++) {
    if (is_prefix(defs[i].info.name,prefix) && is_suffix(defs[i].info.name,suffix)) {
      for (j = 0; j < defs[i].info.member_count; j++) {
        if (memb_struct_check(&defs[i].info.members[j]) && 
          codegen_is_enum(defs[i].info.members[j].type_info.substruct_info.type_name)) {
          for (k = 0; k < l; k++) {
            if (!strcmp(enums[k],defs[i].info.members[j].type_info.substruct_info.type_name)) {
              break;
            } 
          }
          if (k == l) {
            enums[l] = defs[i].info.members[j].type_info.substruct_info.type_name;
            l++;
          }
        }
      }
    }
  }

  for (i = n = 0; i < ndefs; i++) {
    if (defs[i].info.type == ST_ENUM) {
      for (k = 0; k < l; k++) {
        if (!strcmp(enums[k],defs[i].info.name)) {
          filtered_defs[n] = &defs[i];
          n++;
        }
      }
    }
  }

  for (i=0;i<n;i++) {
    fprintf(out,"int %s_to_index(int val) {\n  switch (val) {\n",filtered_defs[i]->info.name);
    codegen_enum_to_index(filtered_defs[i],out);
    fprintf(out,"  }\n  return -1;\n}\n\n");

    fprintf(out,"char *kr_strfr_%s(int val) {\n  switch (val) {\n",filtered_defs[i]->info.name);
    codegen_strfr_enum(filtered_defs[i],out);
    fprintf(out,"  }\n  return NULL;\n}\n\n");
    
    fprintf(out,"int kr_strto_%s(char *string) {\n",filtered_defs[i]->info.name);
    codegen_strto_enum(filtered_defs[i],out);
    fprintf(out,"\n  return -1;\n}\n\n");

  }

  return 0;
}

int codegen_helper_functions(struct_data *defs, int ndefs, char *prefix,
 char *suffix, FILE *out) {

  int i;
  int n;
  struct_data *filtered_defs[ndefs];

  for (i = n = 0; i < ndefs; i++) {
    if (is_prefix(defs[i].info.name,prefix) 
      && is_suffix(defs[i].info.name,suffix) 
      && defs[i].info.type != ST_ENUM) {
      filtered_defs[n] = &defs[i];
      n++;
    }
  }

  for (i = 0; i < n; i++) {
    codegen_helper_init_func(filtered_defs[i],out,defs,ndefs);
    codegen_helper_valid_func(filtered_defs[i],out,defs,ndefs);
    codegen_helper_random_func(filtered_defs[i],out,defs,ndefs);
  }

  return 0;

}