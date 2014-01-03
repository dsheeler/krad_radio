#include "codegen_utils.h"

static char *memb_type_to_json_type(member_info *memb) {

  char *type;

  type = member_type_to_str(memb->type);

  if (!strcmp(type,"int") || !strncmp(type,"int32_t",7)) {
    return "number";
  }

  if (!strcmp(type,"uint") || !strncmp(type,"uint32_t",8)) {
    return "number";
  }

  if (!strncmp(type,"int64_t",7)) {
    return "number";
  }

  if (!strncmp(type,"uint64_t",8)) {
    return "number";
  }

  if (!strncmp(type,"float",5)) {
    return "number";
  }

  if (!strncmp(type,"char",4)) {
    if (memb->arr) {
      return "string";
    } else  if (memb->ptr) {
        if (memb->ptr == 1) {
          return "string";
        } else {
          return NULL;
        }
    } else {
      return "string";
    }
  }

  return NULL;
}

static void codegen_jschema_memb_limits(member_info *memb, FILE *out) {

  member_type type;
  member_type_info *info;

  type = memb->type;
  info = &memb->type_info;

  switch (type) {
    case T_CHAR: {
      if (memb->arr || (memb->ptr == 1)) {
        // TO-DO 
        //fprintf(out,"\"maxLength\" : %d, ",info->info.int_info[1]);
        //fprintf(out,"\"minLength\" : %d, ",info->info.int_info[2]);
      }
      break;
    }
    case T_INT32: {
      fprintf(out,"\"minimum\" : %d, ",info->int32_info.min);
      fprintf(out,"\"maximum\" : %d, ",info->int32_info.max);
      break;
    }
    case T_INT64: {
      fprintf(out,"\"minimum\" : %" PRId64 ", ",info->int64_info.min);
      fprintf(out,"\"maximum\" : %" PRId64 ", ",info->int64_info.max);
      break;
    }
    case T_UINT32: {
      fprintf(out,"\"minimum\" : %u, ",info->uint32_info.min);
      fprintf(out,"\"maximum\" : %u, ",info->uint32_info.max);
      break;
    }
    case T_UINT64: {
      fprintf(out,"\"minimum\" : %" PRIu64 ", ",info->uint64_info.min);
      fprintf(out,"\"maximum\" : %" PRIu64 ", ",info->uint64_info.max);
      break;
    }
    case T_FLOAT: {
     fprintf(out,"\"minimum\" : %0.2f, ",info->float_info.min);
     fprintf(out,"\"maximum\" : %0.2f, ",info->float_info.max);
     break;
   }
   case T_DOUBLE: {
     fprintf(out,"\"minimum\" : %0.2f, ",info->double_info.min);
     fprintf(out,"\"maximum\" : %0.2f, ",info->double_info.max);
     break;
   }
   default : break;
 }

  return;
}

static void codegen_jschema_internal(struct_data *def, struct_data *defs, FILE *out) {

  int j;
  int idx;
  const char *type;

  fprintf(out,"\"type\" : \"object\",");
  fprintf(out,"\"properties\" : {");

  for (j = 0; j < def->info.member_count; j++) {
    if ( (type = memb_type_to_json_type(&def->info.members[j])) ) {

      fprintf(out,"\"%s\" : {",def->info.members[j].name);
      fprintf(out,"\"type\" : \"%s\", ",type);
      codegen_jschema_memb_limits(&def->info.members[j],out);
      fprintf(out,"\"required\" : true ");
      fprintf(out,"}");

      if (j != (def->info.member_count - 1)) {
        fprintf(out,",");
      }

    } else if ( (def->info.members[j].type == T_STRUCT) &&
     (idx = codegen_string_to_enum(def->info.members[j].type_info.substruct_info.type_name)) ) {

      if ((defs[idx-1].info.type != ST_ENUM) && (defs[idx-1].info.type != ST_UNION)) {
        fprintf(out,"\"%s\" : {",def->info.members[j].name);
        codegen_jschema_internal(&defs[idx-1],defs,out);
        fprintf(out,"}");

        if (j != (def->info.member_count - 1)) {
          fprintf(out,",");
        }
      }
    } 
  }

  fprintf(out,"}");

  return;
}

int codegen_jschema(struct_data *defs, int ndefs, 
  char *prefix, char *suffix, FILE *out) {

  int i;
  int n;
  struct_data *filtered_defs[ndefs];

  for (i = n = 0; i < ndefs; i++) {
    if (is_prefix(defs[i].info.name,prefix) && is_suffix(defs[i].info.name,suffix)) {
      filtered_defs[n] = &defs[i];
      n++;
    }
  }

  fprintf(out,"[\n");

  for (i = 0; i < n; i++) {
    fprintf(out,"{");
    fprintf(out,"\"name\" : \"%s\",",filtered_defs[i]->info.name);
    codegen_jschema_internal(filtered_defs[i],defs,out);
    fprintf(out,"}");
    if (i != (n - 1)) {
      fprintf(out,",");
    }
    fprintf(out,"\n");
  }

  fprintf(out,"]\n");

  return 0;
}