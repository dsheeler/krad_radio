#include "codegen_utils.h"

void codegen_json(struct struct_def *def, char *type, FILE *out) {
  int i;
  int last;
  char format[16];
  struct struct_memb_def *members[def->members];

  for (i = last = 0; i < def->members; i++) {
    if (memb_to_print_format(&def->members_info[i],format) 
      || codegen_string_to_enum(def->members_info[i].type)) {
        members[last] = &def->members_info[i];
        last++;
      } else if (def->members_info[i].sub && def->members_info[i].sub->isunion && (i > 0)) {
        members[last] = &def->members_info[i-1];
        members[last+1] = &def->members_info[i];
        last += 2;
      }
  } 


  if (def->isenum) {
    fprintf(out,"  res += snprintf(&%s[res],max-res,\"\\\"%%u\\\"",
      type);
    fprintf(out,"\",*actual);\n");
    return;
  }

  fprintf(out,"  res += snprintf(&%s[res],max-res,\"{\");\n",type);

  for (i = 0; i < last; i++) {
    if (memb_to_print_format(members[i],format)) {
      if ((!members[i]->array && !members[i]->array_str_val) || strchr(format,'s')) {

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

        if (members[i]->array) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",members[i]->array);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",members[i]->array_str_val);
        }

        
        fprintf(out,"    res += snprintf(&%s[res],max-res,\"%s",type,format);
        fprintf(out,"\",actual->%s[i]);\n",members[i]->name);

        if (members[i]->array) {
          fprintf(out,"    if (i != (%d - 1)) {\n",members[i]->array);
        } else {
          fprintf(out,"    if (i != (%s - 1)) {\n",members[i]->array_str_val);
        }

        fprintf(out,"      res += snprintf(&%s[res],max-res,\",\");",type);
        fprintf(out,"\n    }\n");
        fprintf(out,"  }\n");

        fprintf(out,"  res += snprintf(&%s[res],max-res,\"]\");\n",type);

        if (i != (last - 1))
            fprintf(out,"  res += snprintf(&%s[res],max-res,\",\");\n",type);
      }
    } else if (members[i]->sub && members[i]->sub->isunion && (i > 0)) {
      fprintf(out,"  int index;\n");
      fprintf(out,"  index = %s_to_index(actual->%s);\n\n",
        members[i-1]->type,members[i-1]->name);
      fprintf(out,"  switch (index) {\n");
      codegen_union_content_from_type(members[i]->sub,members[i]->name,type,out);
      fprintf(out,"  }\n\n");
    } else if (codegen_string_to_enum(members[i]->type)) {
      char uppercased[strlen(members[i]->type)+1];
      uppercase(members[i]->type,uppercased);
      
      if ((!members[i]->array && !members[i]->array_str_val) || !strncmp(members[i]->type,"char",4)) {
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

        if (members[i]->array) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",members[i]->array);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",members[i]->array_str_val);
        }

        fprintf(out,"    uber.actual = &(actual->%s[i]);\n    uber.type = JSON_%s;\n",
            members[i]->name,uppercased);
        fprintf(out,"    res += info_pack_to_json(&%s[res],&uber,max-res);\n",type);

        if (members[i]->array) {
          fprintf(out,"    if (i != (%d - 1)) {\n",members[i]->array);
        } else {
          fprintf(out,"    if (i != (%s - 1)) {\n",members[i]->array_str_val);
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