#include "codegen_utils.h"

static void codegen_text_union_content_from_type(struct struct_def *def, FILE *out) {
  int i;

  for (i = 0; i < def->members; i++) {
    if (codegen_string_to_enum(def->members_info[i].type)) {
      char uppercased[strlen(def->members_info[i].type)+1];
      uppercase(def->members_info[i].type,uppercased);
      fprintf(out,"    case %d: {\n",i);
      fprintf(out,"      uber.actual = &(actual->%s);\n      ",
        def->members_info[i].name);
      fprintf(out,"uber.type = TEXT_%s;\n",uppercased);
      fprintf(out,"      res += info_pack_to_text(&text[res],&uber,max-res);\n");
      fprintf(out,"      break;\n    }\n");
    }
    /* TODO: handle unions of primitives (ints,floats,strings...) */
  }
        
  return;
}

void codegen_text(struct struct_def *def, char *type, FILE *out) {
  int i;
  char format[16];

  if (def->isenum) {
    fprintf(out,"  res += snprintf(&%s[res],max-res,\"%s : %%u \\n\",*actual);\n",type,def->name);
    return;
  }

  if (def->isunion) {
    fprintf(out,"  switch (uber_actual->type) {\n");
    codegen_text_union_content_from_type(def,out);
    fprintf(out,"  }\n\n");
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

        fprintf(out,"    res += snprintf(&%s[res],max-res,\"%s[%%d] : %s \\n\",i,actual->%s[i]);\n",
          type,def->members_info[i].name,format,def->members_info[i].name);

        fprintf(out,"  }\n");
      }

    } else if (codegen_is_union(def->members_info[i].type) && (i > 0) 
        && codegen_string_to_enum (def->members_info[i-1].type)) {

      char uppercased[strlen(def->members_info[i].type)+1];
      uppercase(def->members_info[i].type,uppercased);

      fprintf(out,"  index = %s_to_index(actual->%s);\n",
          def->members_info[i-1].type,def->members_info[i-1].name);
 
      fprintf(out,"  uber_sub.type = index;\n");
      fprintf(out,"  uber_sub.actual = &(actual->%s);\n",def->members_info[i].name);

      fprintf(out,"  uber.actual = &(uber_sub);\n  uber.type = TEXT_%s;\n",
       uppercased);
      fprintf(out,"  res += info_pack_to_%s(&%s[res],&uber,max-res);\n",type,type);


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