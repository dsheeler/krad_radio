#include "codegen_utils.h"

static char *memb_to_json_type(member_info *memb) {
  switch(memb->type) {
    case T_CHAR: {
      if (memb->arr) {
        return "JSMN_STRING";
      } else  if (memb->ptr) {
        if (memb->ptr == 1) {
          return NULL;
        } else {
          return NULL;
        }
      } else {
        return "JSMN_PRIMITIVE";
      }
      break;
    }
    case T_STRUCT: return NULL;
    default: return "JSMN_PRIMITIVE";
  }
}

static char *memb_type_to_fun(member_info *memb, char *str, char *array) {

  char *type;

  type = member_type_to_str(memb->type);

  if (!strcmp(type,"int") || !strncmp(type,"int32_t",7)) {
    if (!memb->ptr)
      sprintf(str,"actual->%s%s = atoi(&json[tokens[k].start]);",memb->name,array);
      return str;
  }

  if (!strcmp(type,"uint") || !strncmp(type,"uint32_t",8)) {
    if (!memb->ptr)
      sprintf(str,"actual->%s%s = atoi(&json[tokens[k].start]);",memb->name,array);
      return str;
  }

  if (!strncmp(type,"int64_t",7)) {
    if (!memb->ptr)
      sprintf(str,"actual->%s%s = atoi(&json[tokens[k].start]);",memb->name,array);
      return str;
  }

  if (!strncmp(type,"uint64_t",8)) {
    if (!memb->ptr)
      sprintf(str,"actual->%s%s = atoi(&json[tokens[k].start]);",memb->name,array);
      return str;
  }

  if (!strncmp(type,"float",5)) {
    if (!memb->ptr)
      sprintf(str,"actual->%s%s = atof(&json[tokens[k].start]);",memb->name,array);
      return str;
  }

  if (!strncmp(type,"char",4)) {
    if (memb->arr) {
      sprintf(str,"snprintf(actual->%s%s, sizeof(actual->%s%s), \"%%s\", &json[tokens[k].start]);",
        memb->name,array,memb->name,array);
      return str;
    } else  if (memb->ptr) {
        if (memb->ptr == 1) {
          sprintf(str,"actual->%s%s = strdup(&json[tokens[k].start]);",memb->name,array);
          return str;
        } else {
          str[0] = '\0';
          return str;
        }
    } else {
      sprintf(str,"actual->%s%s = atoi(&json[tokens[k].start]);",memb->name,array);
      return str;
    }
  }

  str[0] = '\0';
  return str;
}

void codegen_dejson(struct_data *def, char *type, FILE *out) {

  int i;
  member_info *memb;
  char *format;
  char str[256];

  fprintf(out,"  jsmn_init(&parser);\n  err = jsmn_parse(&parser,json,tokens,%d);\n  ntokens = parser.toknext;\n\n",JSON_MAX_TOKENS);
  fprintf(out,"  k = 0;\n\n");
  fprintf(out,"  if (err != JSMN_SUCCESS || ntokens < 3) {\n    return -1;\n  }\n\n");
  fprintf(out,"  if (tokens[k].type != JSMN_OBJECT) {\n    return -1;\n  }\n\n  k++;\n\n");

  if (def->info.type == ST_ENUM) {
    return;
  }

  if (def->info.type == ST_UNION) {
    //todo
    return;
  }

  for (i = 0; i < def->info.member_count; i++) {

    memb = &def->info.members[i];

    if ( (format = memb_to_json_type(memb)) || ((memb->type == T_STRUCT) && codegen_is_enum(memb->type_info.substruct_info.type_name))) {

        fprintf(out,"  if (ntokens > k && tokens[k].type != JSMN_STRING) {\n    return -%d;\n  }\n",
          i+1);
        fprintf(out,"  json[tokens[k].end] = '\\0';\n");
        fprintf(out,"  if (strncmp(&json[tokens[k].start],\"%s\",%zd)) {\n    return -%d;\n  }\n\n",
          memb->name,strlen(memb->name),i+1);
        fprintf(out,"  k++;\n\n");

      if ((!memb->arr && !memb->len_def[0]) || memb->type == T_CHAR) {

        if ((memb->type == T_STRUCT) && codegen_is_enum(memb->type_info.substruct_info.type_name)) {
          fprintf(out,"  if (ntokens > k && tokens[k].type != JSMN_PRIMITIVE) {\n    return -%d;\n  }\n",i+1);
          fprintf(out,"  json[tokens[k].end] = '\\0';\n");
          fprintf(out,"  actual->%s = atoi(&json[tokens[k].start]);\n",memb->name);
        } else {
          fprintf(out,"  if (ntokens > k && tokens[k].type != %s) {\n    return -%d;\n  }\n\n",
            format,i+1);
          fprintf(out,"  json[tokens[k].end] = '\\0';\n");
          fprintf(out,"  %s\n",memb_type_to_fun(memb,str,""));
        }

        fprintf(out,"  k++;\n\n");

      } else {
        fprintf(out,"  if (ntokens > k && tokens[k].type != JSMN_ARRAY) {\n    return -%d;\n  }\n\n",
          i+1);

        fprintf(out,"  k++;\n");

        if (memb->arr) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",memb->arr);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",memb->len_def);
        }

        fprintf(out,"    if (ntokens > k && tokens[k].type != %s) {\n      return -%d;\n    }\n\n",
          format,i+1);
        fprintf(out,"    json[tokens[k].end] = '\\0';\n");
        fprintf(out,"    %s\n",memb_type_to_fun(memb,str,"[i]"));
        fprintf(out,"    k++;\n");
        fprintf(out,"  }\n\n");
      }

    } else if (memb->type == T_STRUCT && codegen_is_union(memb->type_info.substruct_info.type_name)) {

      if (i > 0) {
        
      }


    } else if (memb->type == T_STRUCT) { 

      char uppercased[strlen(memb->type_info.substruct_info.type_name)+1];
      uppercase(memb->type_info.substruct_info.type_name,uppercased);

      fprintf(out,"  if (ntokens > k && tokens[k].type != JSMN_STRING) {\n    return -%d;\n  }\n\n",
        i+1);
      fprintf(out,"  json[tokens[k].end] = '\\0';\n");
      fprintf(out,"  if (strncmp(&json[tokens[k].start],\"%s\",%zd)) {\n    return -%d;\n  }\n\n",
        memb->name,strlen(memb->name),i+1);
      fprintf(out,"  k++;\n\n");

      if ((!memb->arr && !memb->len_def[0])) {
        fprintf(out,"  if (ntokens > k && tokens[k].type != JSMN_OBJECT) {\n    return -%d;\n  }\n\n",
          i+1);
        fprintf(out,"  uber.actual = &(actual->%s);\n  uber.type = DEJSON_%s;\n",
          memb->name,uppercased);
        fprintf(out,"  json[tokens[k].end] = '\\0';\n");
        fprintf(out,"  res = info_unpack_fr_json(&json[tokens[k].start],&uber);\n");
        fprintf(out,"  if (res < 0) {\n    return -%d;\n  }\n\n",i+1);
        fprintf(out,"  k += res;\n\n");
      } else {

        fprintf(out,"  if (ntokens > k && tokens[k].type != JSMN_ARRAY) {\n    return -%d;\n  }\n\n",
          i+1);
        fprintf(out,"  k++;\n\n");

        if (memb->arr) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",memb->arr);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",memb->len_def);
        }

        fprintf(out,"    if (ntokens > k && tokens[k].type != JSMN_OBJECT) {\n      return -%d;\n    }\n\n",
          i+1);
        fprintf(out,"    uber.actual = &(actual->%s[i]);\n    uber.type = DEJSON_%s;\n",
          memb->name,uppercased);
        fprintf(out,"    json[tokens[k].end] = '\\0';\n");
        fprintf(out,"    res = info_unpack_fr_json(&json[tokens[k].start],&uber);\n");
        fprintf(out,"    if (res < 0) {\n      return -%d;\n    }\n\n",i+1);
        fprintf(out,"    k += res;\n\n");
        fprintf(out,"  }\n\n");
      }

    }

  }

  fprintf(out,"  res = k;\n");

  return;

}
