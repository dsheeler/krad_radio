#include "structs_gathering.h"

void print_usage(char *cmd) {
  printf("syntax:\n%s path prefix suffix format\n",cmd);
  printf("Available formats:\n");
  printf("names --> prints structure names only.\n");
  printf("info --> prints info about the structs matched.\n");
  printf("header --> prints the body of the structs matched.\n");
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

  if (print_structs_defs(defs,ndefs,argv[2],argv[3],argv[4])) {
    fprintf(stderr,"Invalid print format!\n");
    print_usage(argv[0]);
    return 0;
  }

}
