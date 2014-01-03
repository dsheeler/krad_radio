#include "codegen_utils.h"

char *ebml_pack_or_unpack(uint8_t type) {
  if (type) {
    return "kr_ebml_pack";
  } else {
    return "kr_ebml2_unpack_element";
  }
}

char *memb_type_to_str(member_info *memb) {

  char *type;

  type = member_type_to_str(memb->type);

  if (!strcmp(type,"int") || !strncmp(type,"int32_t",7)) {
    if (!memb->ptr)
      return "_int32";
  }

  if (!strcmp(type,"uint") || !strncmp(type,"uint32_t",8)) {
    if (!memb->ptr)
      return "_uint32";
  }

  if (!strncmp(type,"int64_t",7)) {
    if (!memb->ptr)
      return "_int64";
  }

  if (!strncmp(type,"uint64_t",8)) {
    if (!memb->ptr)
      return "_uint64";
  }

  if (!strncmp(type,"float",5)) {
    if (!memb->ptr)
      return "_float";
  }

  if (!strncmp(type,"char",4)) {
    if (memb->arr) {
      return "_string";
    } else  if (memb->ptr) {
        if (memb->ptr == 1) {
          return "_string";
        } else {
          return NULL;
        }
    } else {
      return "_int8";
    }
  }

  return NULL;
}

static void codegen_ebml_union_content_from_type(struct_data *def, 
  char *name, int type, FILE *out) {
  int i;

  for (i = 0; i < def->info.member_count; i++) {
    if (memb_struct_check(&def->info.members[i])) {
      char uppercased[strlen(def->info.members[i].type_info.substruct_info.type_name)+1];
      uppercase(def->info.members[i].type_info.substruct_info.type_name,uppercased);
      fprintf(out,"    case %d: {\n",i);

      fprintf(out,"      uber.actual = &(actual->%s);\n      ",
        def->info.members[i].name);

      if (type) {
        fprintf(out,"uber.type = EBML_%s;\n",uppercased);
        fprintf(out,"      res += info_pack_to_ebml(&ebml[res],&uber);\n");
      } else {
        fprintf(out,"uber.type = DEBML_%s;\n",uppercased);
        fprintf(out,"      res += info_unpack_fr_ebml(&ebml[res],&uber);\n");
      }

      fprintf(out,"      break;\n    }\n");
    }
    /* TODO: handle unions of primitives (ints,floats,strings...) */
  }
        
  return;
}

void codegen_ebml(struct_data *def, char *type, ebml_ftype ebml_fun_type, FILE *out) {

  int i;
  int last;
  char *ebml_fname;
  member_info *members[def->info.member_count];

  if (def->info.type == ST_ENUM) {
    if (ebml_fun_type) {
      fprintf(out,"  res += %s(ebml, 0x%x, *actual);\n","kr_ebml_pack_int32",0xE1);
    } else {
      fprintf(out,"  res += %s(ebml, NULL, (int32_t *)actual);\n","kr_ebml2_unpack_element_int32");
    }
    return;
  }

  if (def->info.type == ST_UNION) {
    fprintf(out,"  switch (uber_actual->type) {\n");
    codegen_ebml_union_content_from_type(def,def->info.name,ebml_fun_type,out);
    fprintf(out,"  }\n\n");
    return;
  }

  for (i = last = 0; i < def->info.member_count; i++) {
    if (memb_type_to_str(&def->info.members[i]) 
      || memb_struct_check(&def->info.members[i])) {
        members[last] = &def->info.members[i];
        last++;
    } 
  } 

  for (i = 0; i < last; i++) {
    if ( (ebml_fname = memb_type_to_str(members[i])) ) {
      if ((members[i]->arr || members[i]->len_def[0]) && members[i]->type != T_CHAR) {
        if (members[i]->arr) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",members[i]->arr);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",members[i]->len_def);
        }

        if (ebml_fun_type) {
          fprintf(out,"    res += %s%s(ebml, 0x%x, actual->%s[i]);\n",
            ebml_pack_or_unpack(ebml_fun_type),ebml_fname,0xE1,members[i]->name);
        } else {
          fprintf(out,"    res += %s%s(ebml, NULL, &actual->%s[i]);\n",
            ebml_pack_or_unpack(ebml_fun_type),ebml_fname,members[i]->name);
        }

        fprintf(out,"  }\n");
      } else if (members[i]->type == T_CHAR) {
        if (ebml_fun_type) {
          fprintf(out,"  res += %s%s(ebml, 0x%x, actual->%s);\n",
            ebml_pack_or_unpack(ebml_fun_type),ebml_fname,0xE1,members[i]->name);
        } else {
          fprintf(out,"  res += %s%s(ebml, NULL, actual->%s , sizeof(actual->%s));\n",
            ebml_pack_or_unpack(ebml_fun_type),ebml_fname,members[i]->name,members[i]->name);
        }
      }
      else {
        if (ebml_fun_type) {
          fprintf(out,"  res += %s%s(ebml, 0x%x, actual->%s);\n",
            ebml_pack_or_unpack(ebml_fun_type),ebml_fname,0xE1,members[i]->name);
        } else {
          fprintf(out,"  res += %s%s(ebml, NULL, &actual->%s);\n",
            ebml_pack_or_unpack(ebml_fun_type),ebml_fname,members[i]->name);
        }
      }
    } else if ( (def->info.members[i-1].type == T_STRUCT && 
      codegen_is_enum(def->info.members[i-1].type_info.substruct_info.type_name))
       && memb_struct_check(&def->info.members[i]) &&
      codegen_is_union(def->info.members[i].type_info.substruct_info.type_name) && (i > 0)) {

      char uppercased[strlen(members[i]->type_info.substruct_info.type_name)+1];
      uppercase(members[i]->type_info.substruct_info.type_name,uppercased);

      fprintf(out,"  index = %s_to_index(actual->%s);\n",
          members[i-1]->type_info.substruct_info.type_name,members[i-1]->name);
 
      fprintf(out,"  uber_sub.type = index;\n");
      fprintf(out,"  uber_sub.actual = &(actual->%s);\n",members[i]->name);

      if (ebml_fun_type) {
        fprintf(out,"  uber.actual = &(uber_sub);\n  uber.type = EBML_%s;\n",
         uppercased);
        fprintf(out,"  res += info_pack_to_%s(&%s[res],&uber);\n",type,type);
      } else {
        fprintf(out,"  uber.actual = &(uber_sub);\n  uber.type = DEBML_%s;\n",
          uppercased);
        fprintf(out,"  res += info_unpack_fr_%s(&%s[res],&uber);\n",&type[1],&type[1]);
      }


    } else if (memb_struct_check(members[i])) {
      char uppercased[strlen(members[i]->type_info.substruct_info.type_name)+1];
      uppercase(members[i]->type_info.substruct_info.type_name,uppercased);

      if ((members[i]->arr || members[i]->len_def[0]) && members[i]->type != T_CHAR) {
        if (members[i]->arr) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",members[i]->arr);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",members[i]->len_def);
        }
        if (ebml_fun_type) {
          fprintf(out,"    uber.actual = &(actual->%s[i]);\n    uber.type = EBML_%s;\n",
            members[i]->name,uppercased);
          fprintf(out,"    res += info_pack_to_%s(&%s[res],&uber);\n",type,type);
        } else {
          fprintf(out,"    uber.actual = &(actual->%s[i]);\n    uber.type = DEBML_%s;\n",
            members[i]->name,uppercased);
          fprintf(out,"    res += info_unpack_fr_%s(&%s[res],&uber);\n",&type[1],&type[1]);
        }
        fprintf(out,"  }\n");

      } else {
        if (ebml_fun_type) {
          fprintf(out,"  uber.actual = &(actual->%s);\n  uber.type = EBML_%s;\n",
            members[i]->name,uppercased);
          fprintf(out,"  res += info_pack_to_%s(&%s[res],&uber);\n",type,type);
        } else {
          fprintf(out,"  uber.actual = &(actual->%s);\n  uber.type = DEBML_%s;\n",
            members[i]->name,uppercased);
          fprintf(out,"  res += info_unpack_fr_%s(&%s[res],&uber);\n",&type[1],&type[1]);
        }
      }
    }
  }

  return;
}