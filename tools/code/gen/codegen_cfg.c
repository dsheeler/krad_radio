#include "codegen_utils.h"

void uppercase(char *str1, char *str2) {

  uint32_t len;
  uint32_t i;

  len = strlen(str1);

  for(i=0;i<len;i++) {
    str2[i] = toupper(str1[i]);
  }

  str2[len] = '\0';

  return;
}

char *memb_type_to_fun(struct struct_memb_def *memb, char *str) {

  if (!strcmp(memb->type,"int") || !strncmp(memb->type,"int32_t",7)) {
    if (!memb->pointer)
      sprintf(str,"params->%s = atoi(config->value);",memb->name);
      return str;
  }

  if (!strcmp(memb->type,"uint") || !strncmp(memb->type,"uint32_t",8)) {
    if (!memb->pointer)
      sprintf(str,"params->%s = atoi(config->value);",memb->name);
      return str;
  }

  if (!strncmp(memb->type,"int64_t",7)) {
    if (!memb->pointer)
      sprintf(str,"params->%s = atoi(config->value);",memb->name);
      return str;
  }

  if (!strncmp(memb->type,"uint64_t",8)) {
    if (!memb->pointer)
      sprintf(str,"params->%s = atoi(config->value);",memb->name);
      return str;
  }

  if (!strncmp(memb->type,"float",5)) {
    if (!memb->pointer)
      sprintf(str,"params->%s = atof(config->value);",memb->name);
      return str;
  }

  if (!strncmp(memb->type,"char",4)) {
    if (memb->array) {
      sprintf(str,"snprintf(params->%s, sizeof(params->%s), \"%%s\", config->value);",memb->name,memb->name);
      return str;
    } else  if (memb->pointer) {
        if (memb->pointer == 1) {
          sprintf(str,"params->%s = strdup(config->value);",memb->name);
          return str;
        } else {
          str[0] = '\0';
          return str;
        }
    } else {
      sprintf(str,"params->%s = atoi(config->value);",memb->name);
      return str;
    }
  }

  str[0] = '\0';
  return str;
}

void print_usage(char *cmd) {
  printf("  Generates config handler function for the given params structures.\n");
  printf("  syntax:\n   %s path\n",cmd);
}

void codegen_cfg(struct header_defs *hdefs, int ndefs) {
  int i;
  int j;
  int l;
  int total;
  char *targets[32];
  struct header_defs *fhdefs[ndefs];
  FILE *header;
  FILE *out;
  char *p;
  char *pp;
  char fname[256];
  char str[256];
  char *bname;

  for (total = i = 0; i < ndefs; i++) {
    for (j = 0; j < hdefs[i].targets.ntargets; j++) {
      if (hdefs[i].targets.types[j] == CONFIG) {
        printf("Found CONFIG target %s\n",hdefs[i].targets.targets[j]);
        fhdefs[total] = &hdefs[i];
        targets[total] = hdefs[i].targets.targets[j];
        total++;
        break;
      }
    }
  }

  for (i = 0; i < total; i++) {

    pp = strrchr(fhdefs[i]->name,'/');

    if (!pp) {
      fprintf(stderr,"Error reading output file name!\n");
      exit(1);
    }

    pp[0] = '\0';
    sprintf(fname,"%s/gen/%s",fhdefs[i]->name,basename(targets[i]));
    pp[0] = '/';


    header = fopen(fname,"w+");

    if (!header) {
      fprintf(stderr,"Error opening output file!\n");
      exit(1);
    }

    p = strstr(fname,".h");
    p[1] = 'c';
    out = fopen(fname,"w+");
    p[1] = 'h';

    if (!out) {
      fprintf(stderr,"Error opening output file!\n");
      exit(1);
    }

    bname = basename(targets[i]);
    char upp[strlen(bname)+1];
    uppercase(bname,upp);
    upp[strlen(upp)-2] = '\0';
    fprintf(header,"#ifndef %s_CFG_H\n",upp);
    fprintf(header,"#define %s_CFG_H\n",upp);
    fprintf(header,"#include <stdint.h>\n");
    fprintf(out,"#include \"%s\"\n",bname);
    fprintf(out,"#include \"../kr_config/kr_config.c\"\n\n");
    fprintf(header,"#include \"%s\"\n\n",basename(fhdefs[i]->name));

    for (j = 0; j < fhdefs[i]->ndefs; j++) {
      if (is_suffix(fhdefs[i]->defs[j].name,"_params")) {
        if (!fhdefs[i]->defs[j].isenum && !fhdefs[i]->defs[j].isunion) {
          if (fhdefs[i]->defs[j].istypedef) {
            fprintf(header,"int handle_config(%s *params, char *configname);\n",fhdefs[i]->defs[j].name);
            fprintf(out,"int handle_config(%s *params, char *configname) {\n",fhdefs[i]->defs[j].name);
          } else {
            fprintf(header,"int handle_config(struct %s *params, char *configname);\n",fhdefs[i]->defs[j].name);
            fprintf(out,"int handle_config(struct %s *params, char *configname) {\n",fhdefs[i]->defs[j].name);
          }
          fprintf(out,"  kr_cfg *config;\n");
          fprintf(out,"  config = kr_config_open(configname);\n\n");
          fprintf(out,"  if (config == NULL) {\n");
          fprintf(out,"    fprintf(stderr,\"Error reading config!\\n\");\n    return -1;\n  }\n\n");
          for (l = 0; l < fhdefs[i]->defs[j].members; l++) {
            fprintf(out,"  if (config->get_val(config, \"%s\")) {\n",fhdefs[i]->defs[j].members_info[l].name);
            fprintf(out,"    %s\n",memb_type_to_fun(&fhdefs[i]->defs[j].members_info[l],str));
            fprintf(out,"  }\n");
          }
          fprintf(out,"\n  kr_config_close(config);\n");
          fprintf(out,"\n  return 0;\n");
          fprintf(out,"}\n\n");
        }
      }
    }

    fprintf(header,"#endif\n\n");
    fclose(out);
    fclose(header);
  }

  return;
}

int main(int argc, char *argv[]) {

  struct header_defs *hdefs;
  int ndefs;

  if (argc != 2) {
    print_usage(argv[0]);
    return 1;
  }

  hdefs = calloc(MAX_HEADERS,sizeof(struct header_defs));
  ndefs = gather_struct_definitions(hdefs,"kr",argv[1]);

  codegen_cfg(hdefs,ndefs);

  free(hdefs);

  return 0;
}