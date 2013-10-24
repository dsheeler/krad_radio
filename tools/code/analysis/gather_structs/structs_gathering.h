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
#define MAX_MEMB 32

struct struct_memb_def {
  char *name;
  char *type;
  int isarray;
  int ispointer;
};

struct struct_def {
  char *fullpath;
  char *name;
  char definition[MAX_DEF_LENGTH];
  struct struct_memb_def members_info[MAX_MEMB];
  int members;
  int line;
  int istypedef;
};

int is_prefix (const char *str, const char *prefix);
int is_suffix (const char *str, const char *suffix);
int gather_struct_definitions(struct struct_def *defs, 
  char *match, char *dir);
int print_structs_defs(struct struct_def *defs, 
  int ndefs, char *prefix, char *suffix, char *format);
