#include "codegen_utils.h"

int codegen_sizeof(struct struct_def *defs, int ndefs, 
  char *prefix, char *suffix, FILE *fp) {

  int i;
  int j;
  char *base;
  
  for (i=j=0;i<ndefs;i++) {
    if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix)) {
      base = basename(defs[i].fullpath);
      if (j == 0) {
        fprintf(fp,"#include \"%s\"\n",base);
        j++;
      } else {
        if (strcmp(base,basename(defs[i-1].fullpath))) {
          fprintf(fp,"#include \"%s\"\n",base);
        }
      }
    }
  }

  fprintf(fp,"\nint main(int argc, char *argv[]) {\n\n");

  for (i=0;i<ndefs;i++) {
    if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix)) {
      if (defs[i].istypedef) {
        fprintf(fp,"%s %s_real;\n",defs[i].name,defs[i].name);
      } else {
        fprintf(fp,"struct %s %s_real;\n",defs[i].name,defs[i].name);
      }
    }
  }

  fprintf(fp,"\n");

  for (i=0;i<ndefs;i++) {
    if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix)) {
      fprintf(fp,"printf(\"\\nName: %s\\nSize: %%d\\n\",sizeof(%s_real));\n",defs[i].name,defs[i].name);
    }
  }

  fprintf(fp,"\n}\n");

  return 0;
}
