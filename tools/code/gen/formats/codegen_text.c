#include "codegen_utils.h"

static void codegen_text_union_content_from_type(struct_data *def, FILE *out) {
  int i;

  for (i = 0; i < def->info.member_count; i++) {
    if (memb_struct_check(&def->info.members[i])) {
      char uppercased[strlen(def->info.members[i].type_info.substruct_info.type_name)+1];
      uppercase(def->info.members[i].type_info.substruct_info.type_name,uppercased);
      fprintf(out,"    case %d: {\n",i);
      fprintf(out,"      uber.actual = &(actual->%s);\n      ",
        def->info.members[i].name);
      fprintf(out,"uber.type = TEXT_%s;\n",uppercased);
      fprintf(out,"      res += info_pack_to_text(&text[res],&uber,max-res);\n");
      fprintf(out,"      break;\n    }\n");
    }
    /* TODO: handle unions of primitives (ints,floats,strings...) */
  }
        
  return;
}

void codegen_text(struct_data *def, char *type, FILE *out) {
  int i;
  char format[16];

  if (def->info.type == ST_ENUM) {
    fprintf(out,"  res += snprintf(&%s[res],max-res,\"%s : %%u \\n\",*actual);\n",type,def->info.name);
    return;
  }

  if (def->info.type == ST_UNION) {
    fprintf(out,"  switch (uber_actual->type) {\n");
    codegen_text_union_content_from_type(def,out);
    fprintf(out,"  }\n\n");
    return;
  }

  for (i = 0; i < def->info.member_count; i++) {
    if (memb_to_print_format(&def->info.members[i],format)) {

      if ((!def->info.members[i].arr && !def->info.members[i].len_def[0]) || def->info.members[i].type == T_CHAR) {
        fprintf(out,"  res += snprintf(&%s[res],max-res,\"%s : %s \\n\",actual->%s);\n",
          type,def->info.members[i].name,format,def->info.members[i].name);
      } else {
        if (def->info.members[i].arr) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",def->info.members[i].arr);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",def->info.members[i].len_def);
        }

        fprintf(out,"    res += snprintf(&%s[res],max-res,\"%s[%%d] : %s \\n\",i,actual->%s[i]);\n",
          type,def->info.members[i].name,format,def->info.members[i].name);

        fprintf(out,"  }\n");
      }

    } else if (memb_struct_check(&def->info.members[i-1]) && 
      codegen_is_union(def->info.members[i].type_info.substruct_info.type_name) && (i > 0)) {

      char uppercased[strlen(def->info.members[i].type_info.substruct_info.type_name)+1];
      uppercase(def->info.members[i].type_info.substruct_info.type_name,uppercased);

      fprintf(out,"  index = %s_to_index(actual->%s);\n",
          def->info.members[i-1].type_info.substruct_info.type_name,
          def->info.members[i-1].name);
 
      fprintf(out,"  uber_sub.type = index;\n");
      fprintf(out,"  uber_sub.actual = &(actual->%s);\n",def->info.members[i].name);

      fprintf(out,"  uber.actual = &(uber_sub);\n  uber.type = TEXT_%s;\n",
       uppercased);
      fprintf(out,"  res += info_pack_to_%s(&%s[res],&uber,max-res);\n",type,type);


    } else if (memb_struct_check(&def->info.members[i])) {

      char uppercased[strlen(def->info.members[i].type_info.substruct_info.type_name)+1];
      uppercase(def->info.members[i].type_info.substruct_info.type_name,uppercased);

      if ((!def->info.members[i].arr && !def->info.members[i].len_def[0])) {
        fprintf(out,"  uber.actual = &(actual->%s);\n  uber.type = TEXT_%s;\n",
          def->info.members[i].name,uppercased);
        fprintf(out,"  res += info_pack_to_text(&%s[res],&uber,max-res);\n",type);

      } else {
        if (def->info.members[i].arr) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",def->info.members[i].arr);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",def->info.members[i].len_def);
        }

        fprintf(out,"    uber.actual = &(actual->%s[i]);\n    uber.type = TEXT_%s;\n",
          def->info.members[i].name,uppercased);
        fprintf(out,"    res += info_pack_to_text(&%s[res],&uber,max-res);\n",type);

        fprintf(out,"  }\n");
      }

    } 
  }

  return;
}