#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_SIZE 256

typedef struct config_options kr_cfg;

struct config_options {
  FILE *cfg_file;
  char value[MAX_LINE_SIZE];
  int (*get_val) (kr_cfg *cfg, const char *optname);
};

kr_cfg *kr_config_open(const char *path);
void kr_config_close(kr_cfg *config);