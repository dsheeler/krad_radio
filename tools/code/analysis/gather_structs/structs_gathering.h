#define _GNU_SOURCE
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
#include <math.h>
#include <errno.h>

#define MAX_HEADERS 256
#define MAX_DEFS 1024
#define MAX_DEF_LENGTH 4096
#define MAX_MEMB 64

typedef enum {
  MEMB_TYPE_UNKNOWN = 0,
  MEMB_TYPE_INT,
  MEMB_TYPE_UINT,
  MEMB_TYPE_FLOAT,
  MEMB_TYPE_STRING
} memb_type;

struct memb_data_info {
  memb_type type;
  union {
    int int_info[3];
    float float_info[3];
  } info;
};

struct struct_memb_def {
  char *name;
  char *type;
  int array;
  int pointer;
  struct struct_def *sub;
  struct memb_data_info data_info;
};

struct struct_def {
  char *fullpath;
  char *name;
  char definition[MAX_DEF_LENGTH];
  struct struct_memb_def members_info[MAX_MEMB];
  int members;
  int line;
  int istypedef;
  int isunion;
  int isenum;
  int issub;
};

int is_prefix (const char *str, const char *prefix);
int is_suffix (const char *str, const char *suffix);
int gather_struct_definitions(struct struct_def *defs, char *fprefix, char *path);
int print_structs_defs(struct struct_def *defs, 
  int ndefs, char *prefix, char *suffix, char *format);
