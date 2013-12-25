#include "codegen_utils.h"

char *ebml_pack_or_unpack(uint8_t type) {
  if (type) {
    return "kr_ebml_pack";
  } else {
    return "kr_ebml2_unpack_element";
  }
}

char *memb_type_to_str(struct struct_memb_def *memb) {

  if (!strcmp(memb->type,"int") || !strncmp(memb->type,"int32_t",7)) {
    if (!memb->pointer)
      return "_int32";
  }

  if (!strcmp(memb->type,"uint") || !strncmp(memb->type,"uint32_t",8)) {
    if (!memb->pointer)
      return "_uint32";
  }

  if (!strncmp(memb->type,"int64_t",7)) {
    if (!memb->pointer)
      return "_int64";
  }

  if (!strncmp(memb->type,"uint64_t",8)) {
    if (!memb->pointer)
      return "_uint64";
  }

  if (!strncmp(memb->type,"float",5)) {
    if (!memb->pointer)
      return "_float";
  }

  if (!strncmp(memb->type,"char",4)) {
    if (memb->array) {
      return "_string";
    } else  if (memb->pointer) {
        if (memb->pointer == 1) {
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

static void codegen_ebml_union_content_from_type(struct struct_def *def, 
  char *name, int type, FILE *out) {
  int i;

  for (i = 0; i < def->members; i++) {
    if (codegen_string_to_enum(def->members_info[i].type)) {
      char uppercased[strlen(def->members_info[i].type)+1];
      uppercase(def->members_info[i].type,uppercased);
      fprintf(out,"    case %d: {\n",i);

      fprintf(out,"      uber.actual = &(actual->%s);\n      ",
        def->members_info[i].name);

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

void codegen_ebml(struct struct_def *def, char *type, ebml_ftype ebml_fun_type, FILE *out) {

  int i;
  int last;
  char *ebml_fname;
  struct struct_memb_def *members[def->members];

  if (def->isenum) {
    if (ebml_fun_type) {
      fprintf(out,"  res += %s(ebml, 0x%x, *actual);\n","kr_ebml_pack_int32",0xE1);
    } else {
      fprintf(out,"  res += %s(ebml, NULL, actual);\n","kr_ebml2_unpack_element_int32");
    }
    return;
  }

  if (def->isunion) {
    fprintf(out,"  switch (uber_actual->type) {\n");
    codegen_ebml_union_content_from_type(def,def->name,ebml_fun_type,out);
    fprintf(out,"  }\n\n");
    return;
  }

  for (i = last = 0; i < def->members; i++) {
    if (memb_type_to_str(&def->members_info[i]) 
      || codegen_string_to_enum(def->members_info[i].type)) {
        members[last] = &def->members_info[i];
        last++;
      } else if (def->members_info[i].sub && def->members_info[i].sub->isunion && (i > 0)) {
        members[last] = &def->members_info[i-1];
        members[last+1] = &def->members_info[i];
        last += 2;
      }
  } 

  for (i = 0; i < last; i++) {
    if ( (ebml_fname = memb_type_to_str(members[i])) ) {
      if ((members[i]->array || members[i]->array_str_val) && strncmp(members[i]->type,"char",4)) {
        if (members[i]->array) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",members[i]->array);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",members[i]->array_str_val);
        }

        if (ebml_fun_type) {
          fprintf(out,"    res += %s%s(ebml, 0x%x, actual->%s[i]);\n",
            ebml_pack_or_unpack(ebml_fun_type),ebml_fname,0xE1,members[i]->name);
        } else {
          fprintf(out,"    res += %s%s(ebml, NULL, &actual->%s[i]);\n",
            ebml_pack_or_unpack(ebml_fun_type),ebml_fname,members[i]->name);
        }

        fprintf(out,"  }\n");
      } else if (!strncmp(members[i]->type,"char",4)) {
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
    } else if (codegen_is_union(members[i]->type) && (i > 0) && codegen_string_to_enum (members[i-1]->type)) {

      char uppercased[strlen(members[i]->type)+1];
      uppercase(members[i]->type,uppercased);

      fprintf(out,"  index = %s_to_index(actual->%s);\n",
          members[i-1]->type,members[i-1]->name);
 
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


    } else if (codegen_string_to_enum(members[i]->type)) {
      char uppercased[strlen(members[i]->type)+1];
      uppercase(members[i]->type,uppercased);

      if ((members[i]->array || members[i]->array_str_val) && strncmp(members[i]->type,"char",4)) {
        if (members[i]->array) {
          fprintf(out,"  for (i = 0; i < %d; i++) {\n",members[i]->array);
        }
        else {
          fprintf(out,"  for (i = 0; i < %s; i++) {\n",members[i]->array_str_val);
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