#include "codegen_bootstrap_utils.h"

void print_usage(char *cmd) {
  printf("syntax:\n%s path prefix suffix\n",cmd);
}

int main(int argc, char *argv[]) {

  struct struct_def defs[MAX_DEFS];
  int ndefs;
  FILE *header;
  FILE *bstrap_file;

  if (argc != 4) {
    print_usage(argv[0]);
    return 0;
  }

  memset(&defs,0,sizeof(defs));

  ndefs = gather_struct_definitions(defs,"krad",argv[1]);

  header = fopen("bootstrapped.h","w+");
  bstrap_file = fopen("bootstrapped.c","w+");

  if (!header || !bstrap_file) {
    fprintf(stderr,"Invalid output file!\n");
    return 1;
  }

  codegen_bootstrap(defs,ndefs,argv[2],argv[3],header);

  fprintf(header,"\ntypedef struct {\n  int type;\n  void *actual;\n} uber_St;\n\n");

  codegen_enum_utils(defs,ndefs,argv[2],argv[3],bstrap_file);
  
  fclose(header);
  fclose(bstrap_file);

  return 0;
}
