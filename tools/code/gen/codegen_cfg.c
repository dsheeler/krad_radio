#include "codegen_utils.h"

void uppercase(char *str1, char *str2) {

  uint32_t len;
  uint32_t i;

  len = strlen(str1);

  for (i = 0; i < len; i++) {
    str2[i] = toupper(str1[i]);
  }

  str2[len] = '\0';

  return;
}

static char *memb_type_to_fun(member_info *memb, char *str) {

  char *type;

  type = member_type_to_str(memb->type);

  if (!strcmp(type,"int") || !strncmp(type,"int32_t",7)) {
    if (!memb->ptr)
      sprintf(str,"params->%s = atoi(config->value);",memb->name);
      return str;
  }

  if (!strcmp(type,"uint") || !strncmp(type,"uint32_t",8)) {
    if (!memb->ptr)
      sprintf(str,"params->%s = atoi(config->value);",memb->name);
      return str;
  }

  if (!strncmp(type,"int64_t",7)) {
    if (!memb->ptr)
      sprintf(str,"params->%s = atoi(config->value);",memb->name);
      return str;
  }

  if (!strncmp(type,"uint64_t",8)) {
    if (!memb->ptr)
      sprintf(str,"params->%s = atoi(config->value);",memb->name);
      return str;
  }

  if (!strncmp(type,"float",5)) {
    if (!memb->ptr)
      sprintf(str,"params->%s = atof(config->value);",memb->name);
      return str;
  }

  if (!strncmp(type,"char",4)) {
    if (memb->arr) {
      sprintf(str,"snprintf(params->%s, sizeof(params->%s), \"%%s\", config->value);",memb->name,memb->name);
      return str;
    } else  if (memb->ptr) {
        if (memb->ptr == 1) {
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

void codegen_cfg(header_data *hdata, int n) {
  int i;
  int j;
  int l;
  int total;
  char *targets[32];
  header_data *fhdata[n];
  FILE *header;
  FILE *out;
  char *p;
  char *pp;
  char fname[256];
  char str[256];
  char *bname;

  for (total = i = 0; i < n; i++) {
    for (j = 0; j < hdata[i].target_count; j++) {
      if (hdata[i].targets[j].type == CONFIG) {
        printf("Found CONFIG target %s\n",hdata[i].targets[j].path);
        fhdata[total] = &hdata[i];
        targets[total] = hdata[i].targets[j].path;
        total++;
        break;
      }
    }
  }

  for (i = 0; i < total; i++) {

    pp = strrchr(fhdata[i]->path,'/');

    if (!pp) {
      fprintf(stderr,"Error reading output file name!\n");
      exit(1);
    }

    pp[0] = '\0';
    sprintf(fname,"%s/gen/%s",fhdata[i]->path,basename(targets[i]));
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
    fprintf(header,"#include \"%s\"\n\n",basename(fhdata[i]->path));

    for (j = 0; j < fhdata[i]->def_count; j++) {
      if (is_suffix(fhdata[i]->defs[j].info.name,"_params")) {
        if (fhdata[i]->defs[j].info.type == ST_STRUCT) {
          if (fhdata[i]->defs[j].info.is_typedef) {
            fprintf(header,"int handle_config(%s *params, char *configname);\n",fhdata[i]->defs[j].info.name);
            fprintf(out,"int handle_config(%s *params, char *configname) {\n",fhdata[i]->defs[j].info.name);
          } else {
            fprintf(header,"int handle_config(struct %s *params, char *configname);\n",fhdata[i]->defs[j].info.name);
            fprintf(out,"int handle_config(struct %s *params, char *configname) {\n",fhdata[i]->defs[j].info.name);
          }
          fprintf(out,"  kr_cfg *config;\n");
          fprintf(out,"  config = kr_config_open(configname);\n\n");
          fprintf(out,"  if (config == NULL) {\n");
          fprintf(out,"    fprintf(stderr,\"Error reading config!\\n\");\n    return -1;\n  }\n\n");
          for (l = 0; l < fhdata[i]->defs[j].info.member_count; l++) {
            fprintf(out,"  if (config->get_val(config, \"%s\")) {\n",fhdata[i]->defs[j].info.members[l].name);
            fprintf(out,"    %s\n",memb_type_to_fun(&fhdata[i]->defs[j].info.members[l],str));
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

  header_data *hdata;
  int n;
  int i;

  if (argc != 2) {
    print_usage(argv[0]);
    return 1;
  }

  hdata = calloc(MAX_HEADERS,sizeof(header_data));

  for (i = 0; i < MAX_HEADERS; i++) {
    hdata[i].defs = calloc(MAX_HEADER_DEFS,sizeof(struct_data));
    hdata[i].targets = calloc(MAX_TARGETS,sizeof(cgen_target));
  }

  n = gather_struct_definitions(hdata,"kr",argv[1]);

  codegen_cfg(hdata,n);

  for (i = 0; i < MAX_HEADERS; i++) {
    free(hdata[i].defs);
    free(hdata[i].targets);
  }
  free(hdata);

  return 0;
}