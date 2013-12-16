#include "kr_config.h"

/* 
Simple config file reader for Krad , sample syntax:
  opt1 = val1
  opt2 = val2
  opt3 = val3
an option per line, spaces and tabs are cleaned therefore :
  opt = val 
and
  opt=val
are the same.

At this time only the first occurrence of an option is read.
Comments are allowed, # is the comment char. Works only if placed at the start of the line.
*/

static char *clean_string (char *str) {

  char *cleaned;
  int i = 0;
  int j = 0;
  int stop = 0;

  cleaned = calloc (strlen (str)+1,sizeof (char));

  while (str[i] != '\0') {
    if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n') {
      cleaned[j] = str[i];
      j++;
      stop = 1;
    }
    else {
      if (stop) {
        break;
      }
    }
    i++;
  }

  str[i] = '\0';

  return cleaned;
}

void kr_config_close(kr_cfg *cfg) {
  fclose (cfg->cfg_file);
  free (cfg);
  return;
}

static int cfg_get_val(kr_cfg *cfg, const char *optname) {

  char *line;
  size_t maxsize = MAX_LINE_SIZE-1;
  char *pos;
  char *p;
  char opt[MAX_LINE_SIZE];
  int i = 0;
  char *cleaned;

  line = calloc (MAX_LINE_SIZE,sizeof( char));

  rewind (cfg->cfg_file);

  while (getline (&line, &maxsize, cfg->cfg_file) >= 0) {

    if (line[0] == '#') {
      continue;
    }

    pos = strstr (line,"=");

    if (pos) {

      p = line;

      while (p<pos) {

        if ((*p != ' ' && *p != '\t')) {
          opt[i] = *p;
          i++;
        }
        p++;
      }

      opt[i] = '\0';
      i = 0;

      if (!strcmp (optname,opt)) {
        cleaned = clean_string (pos+1);
        sprintf (cfg->value,"%s",cleaned);
        free (cleaned);
        return 1;
      }

    }


  }

  return 0;
}

kr_cfg *kr_config_open(const char *path) {

  FILE *fd;
  kr_cfg *config;

  if (path == NULL) {
    return NULL;
  }

  fd = fopen (path,"r");

  if (!fd) {
    return NULL;
  }

  config = calloc (1,sizeof (kr_cfg));

  config->cfg_file = fd;
  config->get_val = cfg_get_val;

  return config;

}