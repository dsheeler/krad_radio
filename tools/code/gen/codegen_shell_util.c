#include "codegen_utils.h"

void print_usage(char *cmd) {
  printf("syntax:\n%s path prefix suffix format\n",cmd);
  printf("Available formats:\n");
  printf("ebml/proto --> generates ebml function prototypes.\n");
  printf("json/proto --> generates json function prototypes.\n");
  printf("text/proto --> generates text function prototypes.\n");
  printf("ebml/func --> generates ebml function definition.\n");
  printf("json/func --> generates json function definition.\n");
  printf("text/func --> generates text function definition.\n");
  printf("enum --> generates enum.\n");
}

int main(int argc, char *argv[]) {

  struct struct_def defs[MAX_DEFS];
  int ndefs;

  if (argc != 5) {
    print_usage(argv[0]);
    return 0;
  }

  memset(&defs,0,sizeof(defs));

  ndefs = gather_struct_definitions(defs,NULL,argv[1]);

  if (codegen(defs,ndefs,argv[2],argv[3],argv[4],stdout)) {
    fprintf(stderr,"Invalid generation format!\n");
    print_usage(argv[0]);
    return 0;
  }

  return 0;
}
