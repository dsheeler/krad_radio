#include "codegen_utils.h"

static void codegen_json_union_content_from_type(struct_data *def, FILE *out) {
  int i;

  for (i = 0; i < def->info.member_count; i++) {
    if (memb_struct_check(&def->info.members[i])) {
      char uppercased[strlen(def->info.members[i].type_info.substruct_info.type_name)+1];
      uppercase(def->info.members[i].type_info.substruct_info.type_name,uppercased);
      fprintf(out,"    case %d: {\n",i);
      fprintf(out,"      uber.actual = &(actual->%s);\n      ",
        def->info.members[i].name);
      fprintf(out,"uber.type = JSON_%s;\n",uppercased);
      fprintf(out,"      res += info_pack_to_json(&json[res],&uber,max-res);\n");
      fprintf(out,"      break;\n    }\n");
    }
    /* TODO: handle unions of primitives (ints,floats,strings...) */
  }
        
  return;
}

void codegen_json(struct_data *def, char *type, FILE *out) {
  int i;
  int last;
  char format[16];
  member_info *members[def->info.member_count];

  for (i = last = 0; i < def->info.member_count; i++) {
    if (memb_to_print_format(&def->info.members[i],format) 
      || memb_struct_check(&def->info.members[i]) ) {
      members[last] = &def->info.members[i];
      last++;
    } 
  } 

  if (def->info.type == ST_ENUM) {
    fprintf(out,"  res += snprintf(&%s[res],max-res,\"\\\"%%u\\\"",
      type);
    fprintf(out,"\",*actual);\n");
    return;
  }

  if (def->info.type == ST_UNION) {
    fprintf(out,"  switch (uber_actual->type) {\n");
    codegen_json_union_content_from_type(def,out);
    fprintf(out,"  }\n\n");
    return;
  }

  fprintf(out,"  res += snprintf(&%s[res],max-res,\"{\");\n",type);

  for (i = 0; i < last; i++) {
    if (memb_to_print_format(members[i],format)) {
      if ((!members[i]->arr && !members[i]->len_def[0]) || strchr(format,'s')) {

        if (strchr(format,'s')) {
          fprintf(out,"  res += snprintf(&%s[res],max-res,\"\\\"%s\\\" : \\\"%s\\\""
            ,type,members[i]->name,format);
        } else {
          fprintf(out,"  res += snprintf(&%s[res],max-res,\"\\\"%s\\\" : %s"
            ,type,members[i]->name,format);
        }

        if (i != (last - 1))
          fprintf(out,",");
      
        fprintf(out,"\",actual->%s);\n",members[i]->name);
      } else {

        fprintf(out,"  res += snprintf(&%s[res],max-res,\"\\\"%s\\\" : [\");\n",
            type,members[i]->name);

        if (members[i]->arr) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",members[i]->arr);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",members[i]->len_def);
        }

        
        fprintf(out,"    res += snprintf(&%s[res],max-res,\"%s",type,format);
        fprintf(out,"\",actual->%s[i]);\n",members[i]->name);

        if (members[i]->arr) {
          fprintf(out,"    if (i != (%d - 1)) {\n",members[i]->arr);
        } else {
          fprintf(out,"    if (i != (%s - 1)) {\n",members[i]->len_def);
        }

        fprintf(out,"      res += snprintf(&%s[res],max-res,\",\");",type);
        fprintf(out,"\n    }\n");
        fprintf(out,"  }\n");

        fprintf(out,"  res += snprintf(&%s[res],max-res,\"]\");\n",type);

        if (i != (last - 1))
            fprintf(out,"  res += snprintf(&%s[res],max-res,\",\");\n",type);
      }
    } else if ((members[i-1]->type == T_STRUCT && 
      codegen_is_enum(members[i-1]->type_info.substruct_info.type_name))
       && memb_struct_check(members[i]) &&
      codegen_is_union(members[i]->type_info.substruct_info.type_name) && (i > 0)) {


      char uppercased[strlen(members[i]->type_info.substruct_info.type_name)+1];
      uppercase(members[i]->type_info.substruct_info.type_name,uppercased);

      fprintf(out,"  index = %s_to_index(actual->%s);\n",
          members[i-1]->type_info.substruct_info.type_name,
          members[i-1]->name);
 
      fprintf(out,"  uber_sub.type = index;\n");
      fprintf(out,"  uber_sub.actual = &(actual->%s);\n",members[i]->name);

      fprintf(out,"  uber.actual = &(uber_sub);\n  uber.type = JSON_%s;\n",
       uppercased);
      fprintf(out,"  res += info_pack_to_%s(&%s[res],&uber,max-res);\n",type,type);


    } else if (memb_struct_check(members[i])) {
      char uppercased[strlen(members[i]->type_info.substruct_info.type_name)+1];
      uppercase(members[i]->type_info.substruct_info.type_name,uppercased);
      
      if ((!members[i]->arr && !members[i]->len_def[0]) || (members[i]->type == T_CHAR)) {
        fprintf(out,"  res += snprintf(&%s[res],max-res,\"\\\"%s\\\": \");\n",
          type,members[i]->name);
        fprintf(out,"  uber.actual = &(actual->%s);\n  uber.type = JSON_%s;\n",
          members[i]->name,uppercased);
        fprintf(out,"  res += info_pack_to_json(&%s[res],&uber,max-res);\n",
          type);

        if (i != (last - 1))
          fprintf(out,"  res += snprintf(&%s[res],max-res,\",\");\n",type);

      } else {
        
        fprintf(out,"  res += snprintf(&%s[res],max-res,\"\\\"%s\\\" : [\");\n",
            type,members[i]->name);

        if (members[i]->arr) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",members[i]->arr);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",members[i]->len_def);
        }

        fprintf(out,"    uber.actual = &(actual->%s[i]);\n    uber.type = JSON_%s;\n",
            members[i]->name,uppercased);
        fprintf(out,"    res += info_pack_to_json(&%s[res],&uber,max-res);\n",type);

        if (members[i]->arr) {
          fprintf(out,"    if (i != (%d - 1)) {\n",members[i]->arr);
        } else {
          fprintf(out,"    if (i != (%s - 1)) {\n",members[i]->len_def);
        }

        fprintf(out,"      res += snprintf(&%s[res],max-res,\",\");",type);
        fprintf(out,"\n    }\n");
        fprintf(out,"  }\n");

        fprintf(out,"  res += snprintf(&%s[res],max-res,\"]\");\n",type);

        if (i != (last - 1))
            fprintf(out,"  res += snprintf(&%s[res],max-res,\",\");\n",type);
      }
    } 
  }

  fprintf(out,"  res += snprintf(&%s[res],max-res,\"}\");\n",type);

  return;
}