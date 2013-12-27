#include "codegen_utils.h"

static char *memb_to_json_type(struct struct_memb_def *memb) {

  if (!strncmp(memb->type,"int",3) || !strncmp(memb->type,"uint",4) || !strncmp(memb->type,"float",5)) {
    return "JSMN_PRIMITIVE";
  }

  if (!strncmp(memb->type,"char",4)) {
    if (memb->array) {
      return "JSMN_STRING";
    } else  if (memb->pointer) {
        if (memb->pointer == 1) {
          return NULL;
        } else {
          return NULL;
        }
    } else {
      return "JSMN_PRIMITIVE";
    }
  }

  return NULL;
}

static char *memb_type_to_fun(struct struct_memb_def *memb, char *str, char *array) {

  if (!strcmp(memb->type,"int") || !strncmp(memb->type,"int32_t",7)) {
    if (!memb->pointer)
      sprintf(str,"actual->%s%s = atoi(&json[tokens[k].start]);",memb->name,array);
      return str;
  }

  if (!strcmp(memb->type,"uint") || !strncmp(memb->type,"uint32_t",8)) {
    if (!memb->pointer)
      sprintf(str,"actual->%s%s = atoi(&json[tokens[k].start]);",memb->name,array);
      return str;
  }

  if (!strncmp(memb->type,"int64_t",7)) {
    if (!memb->pointer)
      sprintf(str,"actual->%s%s = atoi(&json[tokens[k].start]);",memb->name,array);
      return str;
  }

  if (!strncmp(memb->type,"uint64_t",8)) {
    if (!memb->pointer)
      sprintf(str,"actual->%s%s = atoi(&json[tokens[k].start]);",memb->name,array);
      return str;
  }

  if (!strncmp(memb->type,"float",5)) {
    if (!memb->pointer)
      sprintf(str,"actual->%s%s = atof(&json[tokens[k].start]);",memb->name,array);
      return str;
  }

  if (!strncmp(memb->type,"char",4)) {
    if (memb->array) {
      sprintf(str,"snprintf(actual->%s%s, sizeof(actual->%s%s), \"%%s\", &json[tokens[k].start]);",
        memb->name,array,memb->name,array);
      return str;
    } else  if (memb->pointer) {
        if (memb->pointer == 1) {
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

void codegen_dejson(struct struct_def *def, char *type, FILE *out) {

  int i;
  struct struct_memb_def *memb;
  char *format;
  char str[256];

  fprintf(out,"  jsmn_init(&parser);\n  err = jsmn_parse(&parser,json,tokens,%d);\n  ntokens = parser.toknext;\n\n",JSON_MAX_TOKENS);
  fprintf(out,"  k = 0;\n\n");
  fprintf(out,"  if (err != JSMN_SUCCESS || ntokens < 3) {\n    return -1;\n  }\n\n");
  fprintf(out,"  if (tokens[k].type != JSMN_OBJECT) {\n    return -1;\n  }\n\n  k++;\n\n");

  if (def->isenum) {
    return;
  }

  if (def->isunion) {
    //todo
    return;
  }

  for (i = 0; i < def->members; i++) {

    memb = &def->members_info[i];

    if ( (format = memb_to_json_type(memb)) || codegen_is_enum(memb->type) ) {

        fprintf(out,"  if (ntokens > k && tokens[k].type != JSMN_STRING) {\n    return -%d;\n  }\n",
          i+1);
        fprintf(out,"  json[tokens[k].end] = '\\0';\n");
        fprintf(out,"  if (strncmp(&json[tokens[k].start],\"%s\",%zd)) {\n    return -%d;\n  }\n\n",
          memb->name,strlen(memb->name),i+1);
        fprintf(out,"  k++;\n\n");

      if ((!memb->array && !memb->array_str_val) || !strncmp(memb->type,"char",4)) {

        if (codegen_is_enum(memb->type)) {
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

        if (memb->array) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",memb->array);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",memb->array_str_val);
        }

        fprintf(out,"    if (ntokens > k && tokens[k].type != %s) {\n      return -%d;\n    }\n\n",
          format,i+1);
        fprintf(out,"    json[tokens[k].end] = '\\0';\n");
        fprintf(out,"    %s\n",memb_type_to_fun(memb,str,"[i]"));
        fprintf(out,"    k++;\n");
        fprintf(out,"  }\n\n");
      }

    } else if (codegen_is_union(memb->type)) {

      if (i > 0) {
        
      }


    } else if (codegen_string_to_enum(memb->type)) { 

      char uppercased[strlen(memb->type)+1];
      uppercase(memb->type,uppercased);

      fprintf(out,"  if (ntokens > k && tokens[k].type != JSMN_STRING) {\n    return -%d;\n  }\n\n",
        i+1);
      fprintf(out,"  json[tokens[k].end] = '\\0';\n");
      fprintf(out,"  if (strncmp(&json[tokens[k].start],\"%s\",%zd)) {\n    return -%d;\n  }\n\n",
        memb->name,strlen(memb->name),i+1);
      fprintf(out,"  k++;\n\n");

      if ((!memb->array && !memb->array_str_val)) {
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

        if (memb->array) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",memb->array);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",memb->array_str_val);
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
