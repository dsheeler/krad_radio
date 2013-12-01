#include "codegen_utils.h"

static char *memb_type_to_json_type(struct struct_memb_def *memb) {

  if (!strcmp(memb->type,"int") || !strncmp(memb->type,"int32_t",7)) {
    return "number";
  }

  if (!strcmp(memb->type,"uint") || !strncmp(memb->type,"uint32_t",8)) {
    return "number";
  }

  if (!strncmp(memb->type,"int64_t",7)) {
    return "number";
  }

  if (!strncmp(memb->type,"uint64_t",8)) {
    return "number";
  }

  if (!strncmp(memb->type,"float",5)) {
    return "number";
  }

  if (!strncmp(memb->type,"char",4)) {
    if (memb->array) {
      return "string";
    } else  if (memb->pointer) {
        if (memb->pointer == 1) {
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

static void codegen_jschema_memb_limits(struct memb_data_info *info, FILE *out) {

  switch (info->type) {
     case MEMB_TYPE_UINT: {
      fprintf(out,"\"minimum\" : %u, ",info->info.int_info[1]);
      fprintf(out,"\"maximum\" : %u, ",info->info.int_info[2]);
      break;
     }
     case MEMB_TYPE_FLOAT:{
      fprintf(out,"\"minimum\" : %0.2f, ",info->info.float_info[1]);
      fprintf(out,"\"maximum\" : %0.2f, ",info->info.float_info[2]);
      break;
     }
     case MEMB_TYPE_INT: {
      fprintf(out,"\"minimum\" : %d, ",info->info.int_info[1]);
      fprintf(out,"\"maximum\" : %d, ",info->info.int_info[2]);
      break;
     }
     case MEMB_TYPE_STRING: {
      fprintf(out,"\"maxLength\" : %d, ",info->info.int_info[1]);
      fprintf(out,"\"minLength\" : %d, ",info->info.int_info[2]);
      break;
     }
     default: break;
  }

  return;
}


static void codegen_jschema_internal(struct struct_def *def, struct struct_def *defs, FILE *out) {

  int j;
  int idx;
  const char *type;

  fprintf(out,"\"type\" : \"object\",");
  fprintf(out,"\"properties\" : {");

  for (j = 0; j < def->members; j++) {
    if ( (type = memb_type_to_json_type(&def->members_info[j])) ) {

      fprintf(out,"\"%s\" : {",def->members_info[j].name);
      fprintf(out,"\"type\" : \"%s\", ",type);
      if (def->members_info[j].data_info.type) {
        codegen_jschema_memb_limits(&def->members_info[j].data_info,out);
      }
      fprintf(out,"\"required\" : true ");
      fprintf(out,"}");

      if (j != (def->members - 1)) {
        fprintf(out,",");
      }

    } else if ( (idx = codegen_string_to_enum(def->members_info[j].type)) ) {

      if (!defs[idx-1].isenum && !defs[idx-1].isunion) {
        fprintf(out,"\"%s\" : {",def->members_info[j].name);
        codegen_jschema_internal(&defs[idx-1],defs,out);
        fprintf(out,"}");

        if (j != (def->members - 1)) {
          fprintf(out,",");
        }
      }

    } 

  }

  fprintf(out,"}");

  return;
}

int codegen_jschema(struct struct_def *defs, int ndefs, 
  char *prefix, char *suffix, FILE *out) {

  int i;
  int n;
  struct struct_def *filtered_defs[ndefs];

  for (i = n = 0; i < ndefs; i++) {
    if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix)) {
      filtered_defs[n] = &defs[i];
      n++;
    }
  }

  fprintf(out,"[\n");

  for (i = 0; i < n; i++) {
    fprintf(out,"{");
    fprintf(out,"\"name\" : \"%s\",",filtered_defs[i]->name);
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