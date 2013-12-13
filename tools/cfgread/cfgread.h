#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_SIZE 256

typedef struct config_options cfg_t;

struct config_options {
  FILE *cfg_file;
  char value[MAX_LINE_SIZE];
  int (*get_val) (cfg_t* cfg, const char *optname);
  void (*free) (cfg_t* cfg);
};

cfg_t* cfgread (const char *path);