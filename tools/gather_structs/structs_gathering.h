#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <ctype.h>
#include <errno.h>

#define MAX_HEADERS 256
#define MAX_DEFS 1024
#define MAX_DEF_LENGTH 4096

struct struct_def {
  char *fullpath;
  char *name;
  char definition[MAX_DEF_LENGTH]; 
  int members;
  int line;
};

int gather_struct_definitions(struct struct_def *defs, 
  char *match, char *dir);

int print_structs_defs(struct struct_def *defs, 
  int ndefs, char *prefix, char *suffix, char *format);
