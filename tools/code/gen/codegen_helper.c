#include "codegen_utils.h"

static int is_enum(char *name, struct struct_def *defs, int ndefs) {
  int i;

  for (i = 0; i < ndefs; i++) {
    if (defs[i].isenum) {
      if (!strcmp(name,defs[i].name)) {
        return 1;
      }
    }
  }

  return 0;
}

static void codegen_helper_init_func(struct struct_def *def, FILE *out,
 struct struct_def *defs, int ndefs) {

  int i;
  struct memb_data_info *dinfo;

  if (def->istypedef) {
    fprintf(out,"int %s_init(%s *st) {\n",def->name,def->name);
  } else {
    fprintf(out,"int %s_init(struct %s *st) {\n",def->name,def->name);
  }

  fprintf(out,"  if (st == NULL) {\n    return -1;\n  }\n\n");

  if (def->istypedef) {
    fprintf(out,"  memset(st, 0, sizeof(%s));\n",def->name);
  } else {
    fprintf(out,"  memset(st, 0, sizeof(struct %s));\n",def->name);
  }
  
  for (i = 0; i < def->members; i++) {
    dinfo = &(def->members_info[i].data_info);
    switch (dinfo->type) {
      case MEMB_TYPE_UINT:
      case MEMB_TYPE_INT: {
        fprintf(out,"  st->%s = %d;\n",
          def->members_info[i].name,dinfo->info.int_info[0]);
        break;
      }
      case MEMB_TYPE_FLOAT: {
        fprintf(out,"  st->%s = %0.2ff;\n",
          def->members_info[i].name,dinfo->info.float_info[0]);
        break;
      }
      default: break;
    }
    if (codegen_string_to_enum(def->members_info[i].type)) {
      if (!is_enum(def->members_info[i].type,defs,ndefs)) {
        fprintf(out,"  %s_init(&st->%s);\n",
          def->members_info[i].type,def->members_info[i].name);
      }
    }
  }

  fprintf(out,"\n  return 0;\n}\n\n");

  return;

}

static void codegen_helper_random_func(struct struct_def *def, FILE *out,
 struct struct_def *defs, int ndefs) {

  int i;
  struct memb_data_info *dinfo;

  if (def->istypedef) {
    fprintf(out,"int %s_random(%s *st) {\n",def->name,def->name);
  } else {
    fprintf(out,"int %s_random(struct %s *st) {\n",def->name,def->name);
  }

  fprintf(out,"  if (st == NULL) {\n    return -1;\n  }\n\n");

  if (def->istypedef) {
    fprintf(out,"  memset(st, 0, sizeof(%s));\n",def->name);
  } else {
    fprintf(out,"  memset(st, 0, sizeof(struct %s));\n",def->name);
  }

  fprintf(out,"  struct timeval tv;\n  double scale;\n\n  if (st == NULL) {\n");
  fprintf(out,"    return -1;\n  }\n\n  gettimeofday(&tv, NULL);\n");
  fprintf(out,"  srand(tv.tv_sec + tv.tv_usec * 1000000ul);\n\n");

  for (i = 0; i < def->members; i++) {
    dinfo = &(def->members_info[i].data_info);
    switch (dinfo->type) {
      case MEMB_TYPE_UINT:
      case MEMB_TYPE_INT: {
        fprintf(out,"  scale = (double)%d / RAND_MAX;\n",
          abs(dinfo->info.int_info[1]-dinfo->info.int_info[2]));
        fprintf(out,"  st->%s = %d + floor(rand() * scale);\n",
          def->members_info[i].name,dinfo->info.int_info[1]);
        break;
      }
      case MEMB_TYPE_FLOAT: {
        /* Assuming we have 2 decimal values floats only! */
        fprintf(out,"  scale = (double)%d / RAND_MAX;\n",
          (int)fabs(dinfo->info.float_info[1]-dinfo->info.float_info[2])*100);
        fprintf(out,"  st->%s = (%d + floor(rand() * scale)) / 100;\n",
          def->members_info[i].name,(int)(dinfo->info.float_info[1]*100));
        break;
      }
      default: break;
    }
    if (codegen_string_to_enum(def->members_info[i].type)) {
      if (!is_enum(def->members_info[i].type,defs,ndefs)) {
        fprintf(out,"  %s_random(&st->%s);\n",def->members_info[i].type,
          def->members_info[i].name);
      }
    }
  }

  fprintf(out,"\n  return 0;\n}\n\n");

  return;

}

static void codegen_helper_valid_func(struct struct_def *def, FILE *out,
 struct struct_def *defs, int ndefs) {

  int i;
  struct memb_data_info *dinfo;

  if (def->istypedef) {
    fprintf(out,"int %s_valid(%s *st) {\n",def->name,def->name);
  } else {
    fprintf(out,"int %s_valid(struct %s *st) {\n",def->name,def->name);
  }

  fprintf(out,"  if (st == NULL) {\n    return -1;\n  }\n\n");

  for (i=0; i < def->members; i++) {
    dinfo = &(def->members_info[i].data_info);
    switch (dinfo->type) {
      case MEMB_TYPE_UINT:
      case MEMB_TYPE_INT: {
        fprintf(out,"  if ( (st->%s < %d) || (st->%s > %d) ) {\n    return %d;\n  }\n\n",
          def->members_info[i].name,dinfo->info.int_info[1],
          def->members_info[i].name,dinfo->info.int_info[2],(i + 2) * -1);
        break;
      }
      case MEMB_TYPE_FLOAT: {
        fprintf(out,"  if ( (st->%s < %0.2ff) || (st->%s > %0.2ff) ) {\n    return %d;\n  }\n\n",
          def->members_info[i].name,dinfo->info.float_info[1],
          def->members_info[i].name,dinfo->info.float_info[2],(i + 2) * -1);
        break;
      }
      default: break;
    }
    if (codegen_string_to_enum(def->members_info[i].type)) {
      if (!is_enum(def->members_info[i].type,defs,ndefs)) {
        fprintf(out,"  %s_valid(&st->%s);\n",
          def->members_info[i].type,def->members_info[i].name);
      }
    }
  }

  fprintf(out,"\n  return 0;\n}\n\n");

  return;

}

static void codegen_enum_protos(struct struct_def *defs, int ndefs, 
  char *prefix, char *suffix, FILE *out) {
  int i;
  int j;
  int l;
  int n;
  struct struct_def *filtered_defs[ndefs];

  for (j = n = 0; j < ndefs; j++) {
    if (defs[j].isenum) {
      for (i = 0; i < ndefs; i++) {
        if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix)) {
          for (l = 0; l < defs[i].members; l++) {
            if (!strcmp(defs[i].members_info[l].type,defs[j].name)) {
              filtered_defs[n] = &defs[j];
              n++;
            }
          }
        }
      }
    }
  }

  for (i=0;i<n;i++) {
    fprintf(out,"int %s_to_index(int val);\n",filtered_defs[i]->name);
    fprintf(out,"int kr_strto_%s(char *string);\n",filtered_defs[i]->name);
    fprintf(out,"char *kr_strfr_%s(int val);\n",filtered_defs[i]->name);
  }

  return;
}

void codegen_helpers_prototype(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix, FILE *out) {

  int i;
  int n;
  struct struct_def *filtered_defs[ndefs];

  for (i = n = 0; i < ndefs; i++) {
    if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix) && !defs[i].isenum) {
      filtered_defs[n] = &defs[i];
      n++;
    }
  }

  for (i = 0; i < n; i++) {
    if (filtered_defs[i]->istypedef) {
      fprintf(out,"int %s_init(%s *st);\n",
        filtered_defs[i]->name,filtered_defs[i]->name);
      fprintf(out,"int %s_valid(%s *st);\n",
        filtered_defs[i]->name,filtered_defs[i]->name);
      fprintf(out,"int %s_random(%s *st);\n",
        filtered_defs[i]->name,filtered_defs[i]->name);
    } else {
      fprintf(out,"int %s_init(struct %s *st);\n",
        filtered_defs[i]->name,filtered_defs[i]->name);
      fprintf(out,"int %s_valid(struct %s *st);\n",
        filtered_defs[i]->name,filtered_defs[i]->name);
      fprintf(out,"int %s_random(struct %s *st);\n",
        filtered_defs[i]->name,filtered_defs[i]->name);
    }
  }

  codegen_enum_protos(defs,ndefs,prefix,suffix,out);

  return;
}

static void codegen_enum_to_index(struct struct_def *def, FILE *out) {
  int i;

  for (i = 0; i < def->members; i++) {
    fprintf(out,"    case %s:\n      return %d;\n",def->members_info[i].name,i);
  }

  return;
}

static void codegen_strfr_enum(struct struct_def *def, FILE *out) {
  int i;

  for (i = 0; i < def->members; i++) {
    char lowercased[strlen(def->members_info[i].name)+1];
    lowercase(def->members_info[i].name,lowercased);
    fprintf(out,"    case %s:\n      return %s;\n",def->members_info[i].name,lowercased);
  }

  return;
}

static void codegen_strto_enum(struct struct_def *def, FILE *out) {
  int i;

  for (i = 0; i < def->members; i++) {
    char lowercased[strlen(def->members_info[i].name)+1];
    lowercase(def->members_info[i].name,lowercased);
    fprintf(out,"  if (!strcmp(string,\"%s\")) {\n    return %s;\n  }\n",lowercased,def->members_info[i].name);
  }

  return;
}

int codegen_enum_util_functions(struct struct_def *defs, int ndefs, 
  char *prefix, char *suffix, FILE *out) {

  int i;
  int j;
  int l;
  int n;
  struct struct_def *filtered_defs[ndefs];

  for (j = n = 0; j < ndefs; j++) {
    if (defs[j].isenum) {
      for (i = 0; i < ndefs; i++) {
        if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix)) {
          for (l = 0; l < defs[i].members; l++) {
            if (!strcmp(defs[i].members_info[l].type,defs[j].name)) {
              filtered_defs[n] = &defs[j];
              n++;
            }
          }
        }
      }
    }
  }

  for (i=0;i<n;i++) {
    fprintf(out,"int %s_to_index(int val) {\n  switch (val) {\n",filtered_defs[i]->name);
    codegen_enum_to_index(filtered_defs[i],out);
    fprintf(out,"  }\n  return -1;\n}\n\n");

    fprintf(out,"char *kr_strto_%s(int val) {\n  switch (val) {\n",filtered_defs[i]->name);
    codegen_strfr_enum(filtered_defs[i],out);
    fprintf(out,"  }\n  }\n\n");

    fprintf(out,"int kr_strfr_%s(char *string) {\n",filtered_defs[i]->name);
    codegen_strto_enum(filtered_defs[i],out);
    fprintf(out,"  }\n\n");

  }

  return 0;
}

void codegen_union_content_from_type(struct struct_def *def, 
  char *name, char *format, FILE *out) {
  int i;

  for (i = 0; i < def->members; i++) {
    char uppercased[strlen(def->members_info[i].type)+1];
    uppercase(def->members_info[i].type,uppercased);
    fprintf(out,"    case %d: {\n",i);
    fprintf(out,"      res += snprintf(&%s[res],max,\"\\\"%s\\\": \");\n",
          format,name);
    fprintf(out,"      uber.actual = &(actual->%s.%s);\n      uber.type = CGEN_%s;\n",
      name,def->members_info[i].name,uppercased);
    fprintf(out,"      res += info_pack_to_json(&%s[res],&uber,max-res);\n",
          format);
    fprintf(out,"    }\n");
  }
        
  return;
}

int codegen_helper_functions(struct struct_def *defs, int ndefs, char *prefix,
 char *suffix, FILE *out) {

  int i;
  int n;
  struct struct_def *filtered_defs[ndefs];

  for (i = n = 0; i < ndefs; i++) {
    if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix) && !defs[i].isenum) {
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