#include "sizeof_gen.h"

char *sizeof_gen(struct struct_def *defs, int ndefs, 
  char *prefix, char *suffix, char *output, char *options) {

  int i;
  int j;
  FILE *fp;
  char *base;
  char *cmd;
  int size;
  int pos;

  fp = fopen(output,"w+");

  if (!fp) {
    return NULL;
  }

  base = basename(output);

  if (options) {
    size = snprintf(NULL,0,"gcc %s -g -o %.*s %s\n",base,
      strlen(base)-2,base,options);
  } else {
    size = snprintf(NULL,0,"gcc %s -g -o %.*s\n",base,strlen(base)-2,base);
  }

  size++;
  char out[size];
  
  if (options) {
    size = snprintf(out,size,"gcc %s -g -o %.*s %s\n",base,
      strlen(base)-2,base,options);
  } else {
    size = snprintf(out,size,"gcc %s -g -o %.*s\n",base,strlen(base)-2,base);
  }

  cmd = calloc(size,sizeof(char));
  strncpy(cmd,out,size);
  
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

  return cmd;
}
