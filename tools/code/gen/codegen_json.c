#include "codegen_utils.h"

void codegen_json(struct struct_def *def, char *type, FILE *out) {
  int i;
  int j;
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

  fprintf(out,"  res += snprintf(&%s[res],max,\"{\");\n",type);

  for (i = 0; i < last; i++) {
    if (memb_to_print_format(members[i],format)) {
      if (!members[i]->array) {
        fprintf(out,"  res += snprintf(&%s[res],max,\"\\\"%s\\\" : %s"
          ,type,members[i]->name,format);

        if (i != (last - 1))
          fprintf(out,",");
      
        fprintf(out,"\",actual->%s);\n",members[i]->name);
      } else {
        if (strchr(format,'s')) {
          fprintf(out,"  res += snprintf(&%s[res],max,\"\\\"%s\\\" : \\\"%s\\\""
            ,type,members[i]->name,format);

          if (i != (last - 1))
            fprintf(out,",");
        
          fprintf(out,"\",actual->%s);\n",members[i]->name);
        } else {
          fprintf(out,"  res += snprintf(&%s[res],max,\"\\\"%s\\\" : [\");\n",
            type,members[i]->name);
          for (j = 0; j < members[i]->array; j++) {
            fprintf(out,"  res += snprintf(&%s[res],max,\"%s",type,format);

            if (j != (members[i]->array - 1))
              fprintf(out,",");
          
            fprintf(out,"\",actual->%s[%d]);\n",members[i]->name,j);
          }
          fprintf(out,"  res += snprintf(&%s[res],max,\"]",type);

          if (i != (last - 1))
            fprintf(out,",");
  
          fprintf(out,"\");\n");
        }
        
      }
    } else if (members[i]->sub && members[i]->sub->isunion && (i > 0)) {
      fprintf(out,"  int index;\n");
      fprintf(out,"  index = %s_to_index(actual->%s);\n\n",
        members[i-1]->type,members[i-1]->name);
      fprintf(out,"  switch (index) {\n");
      codegen_union_content_from_type(members[i]->sub,members[i]->name,type,out);
      fprintf(out,"  }\n\n");
    } else if (codegen_string_to_enum(members[i]->type)) {
      if (!members[i]->array) {
        char uppercased[strlen(members[i]->type)+1];
        uppercase(members[i]->type,uppercased);
        fprintf(out,"  res += snprintf(&%s[res],max,\"\\\"%s\\\": \");\n",
          type,members[i]->name);
        fprintf(out,"  uber.actual = &(actual->%s);\n  uber.type = CGEN_%s;\n",
          members[i]->name,uppercased);
        fprintf(out,"  res += info_pack_to_json(&%s[res],&uber,max-res);\n",
          type);

        if (i != (last - 1))
          fprintf(out,"  res += snprintf(&%s[res],max,\",\");\n",type);

      } else {
        fprintf(out,"  res += snprintf(&%s[res],max,\"\\\"%s\\\" : [\");\n",
          type,members[i]->name);
        for (j = 0; j < members[i]->array; j++) {
          char uppercased[strlen(members[i]->type)+1];
          uppercase(members[i]->type,uppercased);

          fprintf(out,"  uber.actual = &(actual->%s[%d]);\n  uber.type = CGEN_%s;\n",
            members[i]->name,j,uppercased);
          fprintf(out,"  res += info_pack_to_json(&%s[res],&uber,max-res);\n",type);

          if (j != (members[i]->array - 1))
            fprintf(out,"  res += snprintf(&%s[res],max,\",\");\n",type);

        }
        fprintf(out,"  res += snprintf(&%s[res],max,\"]",type);

        if (i != (last - 1))
          fprintf(out,",");

        fprintf(out,"\");\n");
      }
    } 
  }

  fprintf(out,"  res += snprintf(&%s[res],max,\"}\");\n",type);

  return;
}