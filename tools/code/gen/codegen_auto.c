#include "codegen_utils.h"

void print_usage(char *cmd) {
  printf("  Automatically generates output targets by reading them from within header files.\n");
  printf("  syntax:\n   %s path prefix suffix common_dir\n",cmd);
}

int is_target_new(char *target, char **targets, int n) {

  int i;

  for (i = 0; i < n; i++) {
    if (!strcmp(target,targets[i])) {
      return 0;
    }
  }

  return 1;
}

void common_gen(struct header_defs *hdefs, 
  int ndefs, char *prefix, char *suffix, char *outpath, FILE *genc) {
  FILE *common;
  char outfilen[256];
  char gformat[256];
  char *format;
  int i;

  sprintf(outfilen,"%s/gen.h",outpath);
  common = fopen(outfilen,"w+");

  if (!common) {
    fprintf(stderr,"Invalid output file!\n");
    exit(1);
  }

  fprintf(common,"#ifndef COMMON_GEN_H\n#define COMMON_GEN_H\n");
  fprintf(common,"#include \"krad_ebmlx.h\"\n\n");
  fprintf(common,"typedef struct {\n  int type;\n  void *actual;\n} uber_St;\n\n");

  for (i = 0; i < MAX_TARGET_TYPES; i++) {
    
    switch (i) {
      case TO_TEXT: format = "text"; break;
      case TO_EBML: format = "ebml"; break;
      case TO_JSON: format = "json"; break;
      case FR_EBML: format = "debml"; break;
      default: format = ""; break;
    }

    sprintf(gformat,"%s/typedef",format);
    codegen(hdefs[0].defs,hdefs[0].ndefs,prefix,suffix,gformat,common);

  }

  fprintf(common,"\n");

  for (i = 0; i < MAX_TARGET_TYPES; i++) {
    codegen_enum(hdefs,ndefs,prefix,suffix,common,i+1);
  }

  fprintf(common,"\n");
  fprintf(common,"#endif\n\n");
  fclose(common);

  return;
}

void type_common_gen(struct header_defs *hdefs, 
  int ndefs, char *prefix, char *suffix, char *outpath) {

  int i;
  FILE *tcommons[MAX_TARGET_TYPES-1];
  char *format;
  gen_format gf;
  char gformat[256];
  char outfilen[256];

  for (i = 1; i < MAX_TARGET_TYPES; i++) {
    switch (i) {
      case TO_TEXT: format = "text"; break;
      case TO_EBML: format = "ebml"; break;
      case TO_JSON: format = "json"; break;
      case FR_EBML: format = "debml"; break;
    }

    sprintf(outfilen,"%s/%s_common.c",outpath,format);
    tcommons[i-1] = fopen(outfilen,"a+");

    if (!tcommons[i-1]) {
      fprintf(stderr,"Invalid output file!\n");
      exit(1);
    }

    fprintf(tcommons[i-1],"#include \"gen.h\"\n");

    if (!strncmp(format,"ebml",4) || !strncmp(format,"debml",5)) {
      fprintf(tcommons[i-1],"#include \"krad_ebmlx.h\"\n");
    }

  }

  for (i = 1; i < MAX_TARGET_TYPES; i++) {
    switch (i) {
      case TO_TEXT: format = "text"; gf = TEXT; break;
      case TO_EBML: format = "ebml"; gf = EBML; break;
      case TO_JSON: format = "json"; gf = JSON; break;
      case FR_EBML: format = "debml"; gf = DEBML; break;
    }

    sprintf(gformat,"%s/typedef",format);
    codegen(hdefs[0].defs,hdefs[0].ndefs,prefix,suffix,gformat,tcommons[i-1]);
    fprintf(tcommons[i-1],"\n");
    codegen_array_func(hdefs,ndefs,prefix,suffix,format,gf,tcommons[i-1]);
    fprintf(tcommons[i-1],"\n");

  }

  for (i = 1; i < MAX_TARGET_TYPES; i++) {
    fclose(tcommons[i-1]);
  }

  return;
}

void files_gen(struct header_defs *hdefs, 
  int ndefs, char *prefix, char *suffix, FILE *genc) {

  FILE *out;
  FILE *header;
  char format[256];
  char format2[256];
  char fname[256];
  char *targets[256];
  char *p;
  char *pp;
  int i;
  int l;
  int ntargets;

  out = NULL;
  ntargets = 0;
  header = NULL;

  for (i = 0; i < ndefs; i++) {
    for (l = 0; l < hdefs[i].targets.ntargets; l++) {
      if (hdefs[i].targets.types[l]) {
        
        pp = strrchr(hdefs[i].name,'/');

        if (!pp) {
          fprintf(stderr,"Error reading output file name!\n");
          exit(1);
        }

        pp[0] = '\0';
        sprintf(fname,"%s/gen/%s",hdefs[i].name,basename(hdefs[i].targets.targets[l]));
        pp[0] = '/';

        header = fopen(fname,"a+");

        if (!header) {
          fprintf(stderr,"Invalid output file!\n");
          exit(1);
        }

        fprintf(header,"#include <stdio.h>\n");
        fprintf(header,"#include <stdint.h>\n");
        fprintf(header,"#include \"gen.h\"\n");

        if (hdefs[i].targets.types[l] == TO_EBML || hdefs[i].targets.types[l] == FR_EBML) {
          fprintf(header,"#include \"krad_ebmlx.h\"\n");
        }

        codegen(hdefs[i].defs,hdefs[i].ndefs,prefix,suffix,"includes",header);
        fclose(header);
      }
    }
  }

  for (i = 0; i < ndefs; i++) {

    printf("checking %s for targets...\n",hdefs[i].name);

    for (l = 0; l < hdefs[i].targets.ntargets; l++) {
      if (hdefs[i].targets.types[l]) {

        pp = strrchr(hdefs[i].name,'/');

        if (!pp) {
          fprintf(stderr,"Error reading output file name!\n");
          exit(1);
        }

        pp[0] = '\0';
        sprintf(fname,"%s/gen/%s",hdefs[i].name,basename(hdefs[i].targets.targets[l]));
        pp[0] = '/';

        printf("  found target %s\n",fname);

        p = strstr(fname,".h");
        if (p) {
          header = fopen(fname,"a+");

          if (!header) {
            fprintf(stderr,"Invalid output file!\n");
            exit(1);
          }

          p[1] = 'c';
        
          out = fopen(fname,"a+");

          if (!out) {
            fprintf(stderr,"Invalid output file!\n");
            exit(1);
          }

          if (is_target_new(hdefs[i].targets.targets[l],targets,ntargets)) {
            fprintf(out,"#include \"%s\"\n\n",basename(hdefs[i].targets.targets[l]));
            char *fname2;
            char *brk;
            fname2 = strdup(fname);
            brk = strstr(fname2,"/gen/");
            brk[0] = '\0';
            brk = strrchr(fname2,'/');
            fprintf(genc,"#include \"..%s/gen/%s\"\n",brk,basename(fname));
            free(fname2);
            targets[ntargets] = hdefs[i].targets.targets[l];
            ntargets++;
          }

          p[1] = 'h';
          
          switch (hdefs[i].targets.types[l]) {
            case TO_EBML: 
            sprintf(format,"ebml/func");
            sprintf(format2,"ebml/proto");
            break;
            case TO_TEXT: 
            sprintf(format,"text/func");
            sprintf(format2,"text/proto");
            break;
            case TO_JSON: 
            sprintf(format,"json/func");
            sprintf(format2,"json/proto");
            break;
            case FR_EBML: 
            sprintf(format,"debml/func");
            sprintf(format2,"debml/proto");
            break;
            case HELPERS: 
            sprintf(format,"helper");
            sprintf(format2,"helper_proto");
            break;
            default: break;
          }

          codegen(hdefs[i].defs,hdefs[i].ndefs,prefix,suffix,format,out);
          codegen(hdefs[i].defs,hdefs[i].ndefs,prefix,suffix,format2,header);

          fclose(out);
          fclose(header);
        }
      }
    }
  }

  return;
}

int main(int argc, char *argv[]) {

  struct header_defs *hdefs;
  int ndefs;
  char *rpath = NULL;
  FILE *genc;
  char gencpath[256];

  if (argc != 5) {
    print_usage(argv[0]);
    return 1;
  }

  hdefs = calloc(MAX_HEADERS,sizeof(struct header_defs));
  ndefs = gather_struct_definitions(hdefs,"krad",argv[1]);

  rpath = realpath(argv[4],NULL);

  if (!rpath) {
    fprintf(stderr,"realpath failed, bad path given\n");
    exit(1);
  }

  sprintf(gencpath,"%s/gen.c",rpath);
  genc = fopen(gencpath,"w+");

  if (!genc) {
    fprintf(stderr,"Error opening file %s\n",gencpath);
    exit(1);
  }

  common_gen(hdefs,ndefs,argv[2],argv[3],rpath,genc);

  type_common_gen(hdefs,ndefs,argv[2],argv[3],rpath);

  files_gen(hdefs,ndefs,argv[2],argv[3],genc);

  fprintf(genc,"#include \"ebml_common.c\"\n");
  fprintf(genc,"#include \"text_common.c\"\n");
  fprintf(genc,"#include \"debml_common.c\"\n");
  fprintf(genc,"#include \"json_common.c\"\n\n");

  free(hdefs);
  fclose(genc);

  return 0;
}