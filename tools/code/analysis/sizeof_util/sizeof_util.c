#include "sizeof_gen.h"

void print_usage(char *cmd) {
  printf("syntax:\n%s path prefix suffix output_file compiler_options\n",cmd);
}

int main(int argc, char *argv[]) {

  struct struct_def defs[MAX_DEFS];
  char *output;
  int ndefs;
  char *cmd;

  if (argc < 4) {
    print_usage(argv[0]);
    return 0;
  }

  memset(&defs,0,sizeof(defs));

  ndefs = gather_struct_definitions(defs,NULL,argv[1]);

  if (argc > 4) {
    output = argv[4];
  } else {
    output = "output.c";
  }

  if (argc < 6) {
    if (!(cmd = sizeof_gen(defs,ndefs,argv[2],argv[3],output,NULL))) {
      fprintf(stderr,"Error: %s\n",strerror(errno));
    }
  } else {
    if (!(cmd = sizeof_gen(defs,ndefs,argv[2],argv[3],output,argv[5]))) {
      fprintf(stderr,"Error: %s\n",strerror(errno));
    }
  }

  printf("%s",cmd);

  free(cmd);

  return 0;
}

