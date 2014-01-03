#include "codegen_utils.h"

static void codegen_helpers_union_content_from_type(struct_data *def, 
  char *fun_name, FILE *out) {
  int i;

  for (i = 0; i < def->info.member_count; i++) {
    if (memb_struct_check(&def->info.members[i])) {

      fprintf(out,"    case %d: {\n",i);
      fprintf(out,"      %s_%s(&st->%s);\n",
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
  member_info *memb;

  if (def->info.type == ST_UNION) {
    if (def->info.is_typedef) {
      fprintf(out,"int %s_init(%s *st, int idx) {\n",def->info.name,def->info.name);
    } else {
      fprintf(out,"int %s_init(struct %s *st, int idx) {\n",def->info.name,def->info.name);
    }
  } else {
    if (def->info.is_typedef) {
      fprintf(out,"int %s_init(%s *st) {\n",def->info.name,def->info.name);
    } else {
      fprintf(out,"int %s_init(struct %s *st) {\n",def->info.name,def->info.name);
    }
  }

  fprintf(out,"  if (st == NULL) {\n    return -1;\n  }\n\n");

  if (def->info.type == ST_UNION) {
    fprintf(out,"  switch (idx) {\n");
    codegen_helpers_union_content_from_type(def,"init",out);
    fprintf(out,"  }\n\n");
    fprintf(out,"\n  return -1;\n}\n\n");
    return;
  }

  if (def->info.is_typedef) {
    fprintf(out,"  memset(st, 0, sizeof(%s));\n",def->info.name);
  } else {
    fprintf(out,"  memset(st, 0, sizeof(struct %s));\n",def->info.name);
  }
  
  for (i = 0; i < def->info.member_count; i++) {

    memb = &def->info.members[i];

    char cmp[sizeof(member_type_info)];
    memset(cmp,0,sizeof(member_type_info));

    if (memcmp(&memb->type_info,cmp,sizeof(member_type_info))) {
      
      switch (def->info.members[i].type) {
        case T_CHAR: {
          if (memb->arr || (memb->ptr == 1)) {
        // TO-DO 
          }
          break;
        }
        case T_INT32: {
          fprintf(out,"  st->%s = %d;\n",
            memb->name,memb->type_info.int32_info.init);
          break;
        }
        case T_INT64: {
          fprintf(out,"  st->%s = %" PRId64 ";\n",
            memb->name,memb->type_info.int64_info.init);
          break;
        }
        case T_UINT32: {
          fprintf(out,"  st->%s = %u;\n",
            memb->name,memb->type_info.uint32_info.init);
          break;
        }
        case T_UINT64: {
          fprintf(out,"  st->%s = %" PRIu64 ";\n",
            memb->name,memb->type_info.uint64_info.init);
          break;
        }
        case T_FLOAT: {
          fprintf(out,"  st->%s = %0.2f;\n",
            memb->name,memb->type_info.float_info.init);
          break;
        }
        case T_DOUBLE: {
          fprintf(out,"  st->%s = %0.2f;\n",
            memb->name,memb->type_info.double_info.init);
          break;
        }
        default : break;
      }

    }

    if (memb_struct_check(memb) && codegen_is_union(memb->type_info.substruct_info.type_name)) {
      if ((i > 0) && memb_struct_check(&def->info.members[i-1])) {
        fprintf(out,"  %s_init(&st->%s,%s_to_index(st->%s));\n",
          memb->type_info.substruct_info.type_name,memb->name,
            def->info.members[i-1].type_info.substruct_info.type_name,def->info.members[i-1].name);
      }
    } else {
      if (memb_struct_check(memb)) {
        if (!codegen_is_enum(memb->type_info.substruct_info.type_name)) {
          fprintf(out,"  %s_init(&st->%s);\n",
            memb->type_info.substruct_info.type_name,memb->name);
        }
      }
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

  memset(cmp,0,sizeof(member_type_info));

  if (def->info.type == ST_UNION) {
    if (def->info.is_typedef) {
      fprintf(out,"int %s_random(%s *st, int idx) {\n",def->info.name,def->info.name);
    } else {
      fprintf(out,"int %s_random(struct %s *st, int idx) {\n",def->info.name,def->info.name);
    }
  } else {
    if (def->info.is_typedef) {
      fprintf(out,"int %s_random(%s *st) {\n",def->info.name,def->info.name);
    } else {
      fprintf(out,"int %s_random(struct %s *st) {\n",def->info.name,def->info.name);
    }
  }

  for (i = 0; i < def->info.member_count; i++) {
    memb = &def->info.members[i];
    if (memcmp(&memb->type_info,cmp,sizeof(member_type_info))) {
      if (memb->type != T_STRUCT) {
        fprintf(out,"  struct timeval tv;\n");
        fprintf(out,"  double scale;\n\n");
        fprintf(out,"  gettimeofday(&tv, NULL);\n  srand(tv.tv_sec + tv.tv_usec * 1000000ul);\n\n");
      }
      break;
    }
  }

  fprintf(out,"  if (st == NULL) {\n    return -1;\n  }\n\n");

  if (def->info.type == ST_UNION) {
    fprintf(out,"  switch (idx) {\n");
    codegen_helpers_union_content_from_type(def,"random",out);
    fprintf(out,"  }\n\n");
    fprintf(out,"\n  return -1;\n}\n\n");
    return;
  }

  if (def->info.is_typedef) {
    fprintf(out,"  memset(st, 0, sizeof(%s));\n",def->info.name);
  } else {
    fprintf(out,"  memset(st, 0, sizeof(struct %s));\n",def->info.name);
  }

  fprintf(out,"  if (st == NULL) {\n    return -1;\n  }\n\n");

  for (i = 0; i < def->info.member_count; i++) {

    memb = &def->info.members[i];

    if (memcmp(&memb->type_info,cmp,sizeof(member_type_info))) {

      switch (def->info.members[i].type) {
        case T_CHAR: {
          if (memb->arr || (memb->ptr == 1)) {
            // TO-DO 
            fprintf(out,"  scale = 0;\n");
          }
          break;
        }
        case T_INT32: {
          fprintf(out,"  scale = (double)%d / RAND_MAX;\n",
            abs(memb->type_info.int32_info.min-memb->type_info.int32_info.max));
          fprintf(out,"  st->%s = %d + floor(rand() * scale);\n",
            memb->name,memb->type_info.int32_info.min);
          break;
        }
        case T_INT64: {
          fprintf(out,"  scale = (double)%" PRId64 " / RAND_MAX;\n",
            labs(memb->type_info.int64_info.min-memb->type_info.int64_info.max));
          fprintf(out,"  st->%s = %" PRId64 " + floor(rand() * scale);\n",
            memb->name,memb->type_info.int64_info.min);
          break;
        }
        case T_UINT32: {
          fprintf(out,"  scale = (double)%u / RAND_MAX;\n",
            abs(memb->type_info.uint32_info.min-memb->type_info.uint32_info.max));
          fprintf(out,"  st->%s = %u + floor(rand() * scale);\n",
            memb->name,memb->type_info.uint32_info.min);
          break;
        }
        case T_UINT64: {
          fprintf(out,"  scale = (double)%" PRIu64 " / RAND_MAX;\n",
            labs(memb->type_info.uint64_info.min-memb->type_info.uint64_info.max));
          fprintf(out,"  st->%s = %" PRIu64 " + floor(rand() * scale);\n",
            memb->name,memb->type_info.uint64_info.min);
          break;
        }
        case T_FLOAT: {
          fprintf(out,"  scale = (double)%0.2f / RAND_MAX;\n",
            fabs(memb->type_info.float_info.min-memb->type_info.float_info.max));
          fprintf(out,"  st->%s = %0.2f + floor(rand() * scale);\n",
            memb->name,memb->type_info.float_info.min);
          break;
        }
        case T_DOUBLE: {
          fprintf(out,"  scale = (double)%0.2f / RAND_MAX;\n",
            fabs(memb->type_info.double_info.min-memb->type_info.double_info.max));
          fprintf(out,"  st->%s = %0.2f + floor(rand() * scale);\n",
            memb->name,memb->type_info.double_info.min);
          break;
        }
        default : break;
      }

    }

    if (memb_struct_check(memb) && codegen_is_union(memb->type_info.substruct_info.type_name)) {
      if ((i > 0) && memb_struct_check(&def->info.members[i-1])) {
        fprintf(out,"  %s_random(&st->%s,%s_to_index(st->%s));\n",
          memb->type_info.substruct_info.type_name,memb->name,
          def->info.members[i-1].type_info.substruct_info.type_name,def->info.members[i-1].name);
      }
    } else {
      if (memb_struct_check(memb)) {
        if (!codegen_is_enum(memb->type_info.substruct_info.type_name)) {
          fprintf(out,"  %s_random(&st->%s);\n",
            memb->type_info.substruct_info.type_name,memb->name);
        }
      }
    }

  }

  fprintf(out,"\n  return 0;\n}\n\n");

  return;

}

static void codegen_helper_valid_func(struct_data *def, FILE *out,
 struct_data *defs, int ndefs) {

  int i;
  member_info *memb;

  if (def->info.type == ST_UNION) {
    if (def->info.is_typedef) {
      fprintf(out,"int %s_valid(%s *st, int idx) {\n",def->info.name,def->info.name);
    } else {
      fprintf(out,"int %s_valid(struct %s *st, int idx) {\n",def->info.name,def->info.name);
    }
  } else {
    if (def->info.is_typedef) {
      fprintf(out,"int %s_valid(%s *st) {\n",def->info.name,def->info.name);
    } else {
      fprintf(out,"int %s_valid(struct %s *st) {\n",def->info.name,def->info.name);
    }
  }

  fprintf(out,"  if (st == NULL) {\n    return -1;\n  }\n\n");

  if (def->info.type == ST_UNION) {
    fprintf(out,"  switch (idx) {\n");
    codegen_helpers_union_content_from_type(def,"valid",out);
    fprintf(out,"  }\n\n");
    fprintf(out,"\n  return -1;\n}\n\n");
    return;
  }

  for (i = 0; i < def->info.member_count; i++) {
    
    memb = &def->info.members[i];

    char cmp[sizeof(member_type_info)];
    memset(cmp,0,sizeof(member_type_info));
    if (memcmp(&memb->type_info,cmp,sizeof(member_type_info))) {

      switch (def->info.members[i].type) {
        case T_CHAR: {
          if (memb->arr || (memb->ptr == 1)) {
        // TO-DO 
          }
          break;
        }
        case T_INT32: {
          fprintf(out,"  if ( (st->%s < %d) || (st->%s > %d) ) {\n    return %d;\n  }\n\n",
            memb->name,memb->type_info.int32_info.min,
            memb->name,memb->type_info.int32_info.max,(i + 2) * -1);
          break;
        }
        case T_INT64: {
          fprintf(out,"  if ( (st->%s < %" PRId64 ") || (st->%s > %" PRId64 ") ) {\n    return %d;\n  }\n\n",
            memb->name,memb->type_info.int64_info.min,
            memb->name,memb->type_info.int64_info.max,(i + 2) * -1);
          break;
        }
        case T_UINT32: {
          fprintf(out,"  if ( (st->%s < %u) || (st->%s > %u) ) {\n    return %d;\n  }\n\n",
            memb->name,memb->type_info.uint32_info.min,
            memb->name,memb->type_info.uint32_info.max,(i + 2) * -1);
          break;
        }
        case T_UINT64: {
          fprintf(out,"  if ( (st->%s < %" PRIu64 ") || (st->%s > %" PRIu64 ") ) {\n    return %d;\n  }\n\n",
            memb->name,memb->type_info.uint64_info.min,
            memb->name,memb->type_info.uint64_info.max,(i + 2) * -1);
          break;
        }
        case T_FLOAT: {
         fprintf(out,"  if ( (st->%s < %0.2f) || (st->%s > %0.2f) ) {\n    return %d;\n  }\n\n",
          memb->name,memb->type_info.float_info.min,
          memb->name,memb->type_info.float_info.max,(i + 2) * -1);
         break;
       }
       case T_DOUBLE: {
         fprintf(out,"  if ( (st->%s < %0.2f) || (st->%s > %0.2f) ) {\n    return %d;\n  }\n\n",
          memb->name,memb->type_info.double_info.min,
          memb->name,memb->type_info.double_info.max,(i + 2) * -1);
         break;
       }
       default : break;
     }

    }

    if (memb_struct_check(memb) && codegen_is_union(memb->type_info.substruct_info.type_name)) {
      if ((i > 0) && memb_struct_check(&def->info.members[i-1])) {
        fprintf(out,"  %s_valid(&st->%s,%s_to_index(st->%s));\n",
          memb->type_info.substruct_info.type_name,memb->name,
          def->info.members[i-1].type_info.substruct_info.type_name,def->info.members[i-1].name);
      }
    } else {
      if (memb_struct_check(memb)) {
        if (!codegen_is_enum(memb->type_info.substruct_info.type_name)) {
          fprintf(out,"  %s_valid(&st->%s);\n",
            memb->type_info.substruct_info.type_name,memb->name);
        }
      }
    }
    
  }

  fprintf(out,"\n  return 0;\n}\n\n");

  return;

}

static void codegen_enum_protos(struct_data *defs, int ndefs, 
  char *prefix, char *suffix, FILE *out) {
/*  int i;
  int j;
  int l;
  int n;
  struct_data *filtered_defs[ndefs];*/

/*  for (j = n = 0; j < ndefs; j++) {
    if (defs[j].isenum) {
      for (i = 0; i < ndefs; i++) {
        if (is_prefix(defs[i].info.name,prefix) && is_suffix(defs[i].info.name,suffix)) {
          for (l = 0; l < defs[i].members; l++) {
            if (!strcmp(defs[i].members_info[l].type,defs[j].name)) {
              filtered_defs[n] = &defs[j];
              n++;
            }
          }
        }
      }
    }
  }*/


/*  for (i = n = 0; i < ndefs; i++) {
    if (is_prefix(defs[i].info.name,prefix) && is_suffix(defs[i].info.name,suffix) && defs[i].isenum) {
      filtered_defs[n] = &defs[i];
      n++;
    }
  }*/

/*  for (i=0;i<n;i++) {
    fprintf(out,"int %s_to_index(int val);\n",filtered_defs[i]->name);
    fprintf(out,"int kr_strto_%s(char *string);\n",filtered_defs[i]->name);
    fprintf(out,"char *kr_strfr_%s(int val);\n",filtered_defs[i]->name);
  }*/

  return;
}

void codegen_helpers_prototype(struct_data *defs, int ndefs, char *prefix,
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
    if (filtered_defs[i]->info.is_typedef) {
      if (filtered_defs[i]->info.type == ST_UNION) {
        fprintf(out,"int %s_init(%s *st, int idx);\n",
          filtered_defs[i]->info.name,filtered_defs[i]->info.name);
        fprintf(out,"int %s_valid(%s *st, int idx);\n",
          filtered_defs[i]->info.name,filtered_defs[i]->info.name);
        fprintf(out,"int %s_random(%s *st, int idx);\n",
          filtered_defs[i]->info.name,filtered_defs[i]->info.name);
      } else {
        fprintf(out,"int %s_init(%s *st);\n",
          filtered_defs[i]->info.name,filtered_defs[i]->info.name);
        fprintf(out,"int %s_valid(%s *st);\n",
          filtered_defs[i]->info.name,filtered_defs[i]->info.name);
        fprintf(out,"int %s_random(%s *st);\n",
          filtered_defs[i]->info.name,filtered_defs[i]->info.name);
      }
    } else {
      if (filtered_defs[i]->info.type == ST_UNION) {
        fprintf(out,"int %s_init(struct %s *st, int idx);\n",
          filtered_defs[i]->info.name,filtered_defs[i]->info.name);
        fprintf(out,"int %s_valid(struct %s *st, int idx);\n",
          filtered_defs[i]->info.name,filtered_defs[i]->info.name);
        fprintf(out,"int %s_random(struct %s *st, int idx);\n",
          filtered_defs[i]->info.name,filtered_defs[i]->info.name);
      } else {
        fprintf(out,"int %s_init(struct %s *st);\n",
          filtered_defs[i]->info.name,filtered_defs[i]->info.name);
        fprintf(out,"int %s_valid(struct %s *st);\n",
          filtered_defs[i]->info.name,filtered_defs[i]->info.name);
        fprintf(out,"int %s_random(struct %s *st);\n",
          filtered_defs[i]->info.name,filtered_defs[i]->info.name);
      }

    }
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