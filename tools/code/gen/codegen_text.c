#include "codegen_utils.h"

void codegen_text(struct struct_def *def, char *type, FILE *out) {
  int i;
  char format[16];

  if (def->isenum) {
    fprintf(out,"  res += snprintf(&%s[res],max-res,\"%s : %%u \\n\",*actual);\n",type,def->name);
    return;
  }

  for (i = 0; i < def->members; i++) {
    if (memb_to_print_format(&def->members_info[i],format)) {

      if ((!def->members_info[i].array && !def->members_info[i].array_str_val) || strchr(format,'s')) {
        fprintf(out,"  res += snprintf(&%s[res],max-res,\"%s : %s \\n\",actual->%s);\n",
          type,def->members_info[i].name,format,def->members_info[i].name);
      } else {
        if (def->members_info[i].array) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",def->members_info[i].array);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",def->members_info[i].array_str_val);
        }

        fprintf(out,"    res += snprintf(&%s[res],max-res,\"%s[%%d] : %s \\n\",actual->%s[i],i);\n",
          type,def->members_info[i].name,format,def->members_info[i].name);

        fprintf(out,"  }\n");
      }

    } else if (def->members_info[i].sub && def->members_info[i].sub->isunion && (i > 0)) {
      fprintf(out,"  int index;\n");
      fprintf(out,"  index = %s_to_index(actual->%s);\n\n",
        def->members_info[i-1].type,def->members_info[i-1].name);
      fprintf(out,"  switch (index) {\n");
      codegen_union_content_from_type(def->members_info[i].sub,
        def->members_info[i].name,type,out);
      fprintf(out,"  }\n\n");
    } else if (codegen_string_to_enum(def->members_info[i].type)) {

      char uppercased[strlen(def->members_info[i].type)+1];
      uppercase(def->members_info[i].type,uppercased);

      if ((!def->members_info[i].array && !def->members_info[i].array_str_val)) {
        fprintf(out,"  uber.actual = &(actual->%s);\n  uber.type = TEXT_%s;\n",
          def->members_info[i].name,uppercased);
        fprintf(out,"  res += info_pack_to_text(&%s[res],&uber,max-res);\n",type);

      } else {
        if (def->members_info[i].array) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",def->members_info[i].array);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",def->members_info[i].array_str_val);
        }

        fprintf(out,"    uber.actual = &(actual->%s[i]);\n    uber.type = TEXT_%s;\n",
          def->members_info[i].name,uppercased);
        fprintf(out,"    res += info_pack_to_text(&%s[res],&uber,max-res);\n",type);

        fprintf(out,"  }\n");
      }

    } 
  }

  return;
}