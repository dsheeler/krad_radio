#include "codegen_utils.h"

char *memb_type_to_ebml_fname(struct struct_memb_def *memb) {

  if (!strcmp(memb->type,"int") || !strncmp(memb->type,"int32_t",7)) {
    return "kr_ebml_pack_int32";
  }

  if (!strcmp(memb->type,"uint") || !strncmp(memb->type,"uint32_t",8)) {
    return "kr_ebml_pack_uint32";
  }

  if (!strncmp(memb->type,"int64_t",7)) {
    return "kr_ebml_pack_int64";
  }

  if (!strncmp(memb->type,"uint64_t",8)) {
    return "kr_ebml_pack_uint64";
  }

  if (!strncmp(memb->type,"float",5)) {
    return "kr_ebml_pack_float";
  }

  if (!strncmp(memb->type,"char",4)) {
    if (memb->array) {
      return "kr_ebml_pack_string";
    } else  if (memb->pointer) {
        if (memb->pointer == 1) {
          return "kr_ebml_pack_string";
        } else {
          return NULL;
        }
    } else {
      return "kr_ebml_pack_int8";
    }
  }

  return NULL;
}

void codegen_ebml(struct struct_def *def, char *type, FILE *out) {

  int i;
  int last;
  char *ebml_fname;
  struct struct_memb_def *members[def->members];

  for (i = last = 0; i < def->members; i++) {
    if (memb_type_to_ebml_fname(&def->members_info[i]) 
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
    if ( (ebml_fname = memb_type_to_ebml_fname(members[i])) ) {
      fprintf(out,"  res += %s(ebml, 0x%x, actual->%s);\n",ebml_fname,0xE1,members[i]->name);
    } else if (codegen_string_to_enum(members[i]->type)) {
      char uppercased[strlen(members[i]->type)+1];
      uppercase(members[i]->type,uppercased);
      fprintf(out,"  uber.actual = &(actual->%s);\n  uber.type = CGEN_%s;\n",
        members[i]->name,uppercased);
      fprintf(out,"  res += info_pack_to_%s(&%s[res],&uber,max-res);\n",type,type);
    }
  }

  return;
}