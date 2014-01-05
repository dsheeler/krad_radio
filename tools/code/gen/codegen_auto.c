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

void common_gen(header_data *hdata, int n, char *prefix, 
  char *suffix, char *outpath, FILE *genc) {
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

  for (i = 0; i < TARGET_TYPES - 1; i++) {
    
    switch (i) {
      case TO_TEXT: format = "text"; break;
      case TO_EBML: format = "ebml"; break;
      case TO_JSON: format = "json"; break;
      case FR_JSON: format = "dejson"; break;
      case FR_EBML: format = "debml"; break;
      default: format = ""; break;
    }

    sprintf(gformat,"%s/typedef",format);
    codegen(hdata[0].defs,hdata[0].def_count,prefix,suffix,gformat,common);

  }

  fprintf(common,"\n");

  for (i = 0; i < TARGET_TYPES - 1; i++) {
    codegen_enum(hdata,n,prefix,suffix,common,i+1);
  }

  fprintf(common,"\n");
  fprintf(common,"#endif\n\n");
  fclose(common);

  return;
}

void type_common_gen(header_data *hdata, int n, char *prefix,
 char *suffix, char *outpath) {

  int i;
  FILE *tcommons[TARGET_TYPES-2];
  char *format;
  gen_format gf;
  char gformat[256];
  char outfilen[256];

  for (i = 1; i < TARGET_TYPES - 1; i++) {
    switch (i) {
      case TO_TEXT: format = "text"; break;
      case TO_EBML: format = "ebml"; break;
      case TO_JSON: format = "json"; break;
      case FR_JSON: format = "dejson"; break;
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

  for (i = 1; i < TARGET_TYPES - 1; i++) {
    switch (i) {
      case TO_TEXT: format = "text"; gf = TEXT; break;
      case TO_EBML: format = "ebml"; gf = EBML; break;
      case TO_JSON: format = "json"; gf = JSON; break;
      case FR_JSON: format = "dejson"; gf = DEJSON; break;
      case FR_EBML: format = "debml"; gf = DEBML; break;
    }

    sprintf(gformat,"%s/typedef",format);
    codegen(hdata[0].defs,hdata[0].def_count,prefix,suffix,gformat,tcommons[i-1]);
    fprintf(tcommons[i-1],"\n");
    codegen_array_func(hdata,n,prefix,suffix,format,gf,tcommons[i-1]);
    fprintf(tcommons[i-1],"\n");

  }

  for (i = 1; i < TARGET_TYPES - 1; i++) {
    fclose(tcommons[i-1]);
  }

  return;
}

void files_gen(header_data *hdata, 
  int n, char *prefix, char *suffix, FILE *genc) {

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

  for (i = 0; i < n; i++) {
    for (l = 0; l < hdata[i].target_count; l++) {
      if (hdata[i].targets[l].type) {
        
        pp = strrchr(hdata[i].path,'/');

        if (!pp) {
          fprintf(stderr,"Error reading output file name!\n");
          exit(1);
        }

        pp[0] = '\0';
        sprintf(fname,"%s/gen/%s",hdata[i].path,basename(hdata[i].targets[l].path));
        pp[0] = '/';

        header = fopen(fname,"a+");

        if (!header) {
          fprintf(stderr,"Invalid output file!\n");
          exit(1);
        }

        fprintf(header,"#include <stdio.h>\n");
        fprintf(header,"#include <stdint.h>\n");
        fprintf(header,"#include \"gen.h\"\n");

        if (hdata[i].targets[l].type == TO_EBML || hdata[i].targets[l].type == FR_EBML) {
          fprintf(header,"#include \"krad_ebmlx.h\"\n");
        }

        if (hdata[i].targets[l].type == FR_JSON) {
          fprintf(header,"#include \"jsmn.h\"\n");
        }
        codegen(hdata[i].defs,hdata[i].def_count,prefix,suffix,"includes",header);
        fclose(header);
      }
    }
  }

  for (i = 0; i < n; i++) {

    printf("checking %s for targets...\n",hdata[i].path);

    for (l = 0; l < hdata[i].target_count; l++) {
      if (hdata[i].targets[l].type) {

        pp = strrchr(hdata[i].path,'/');

        if (!pp) {
          fprintf(stderr,"Error reading output file name!\n");
          exit(1);
        }

        pp[0] = '\0';
        sprintf(fname,"%s/gen/%s",hdata[i].path,basename(hdata[i].targets[l].path));
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

          if (is_target_new(hdata[i].targets[l].path,targets,ntargets)) {
            fprintf(out,"#include \"%s\"\n\n",basename(hdata[i].targets[l].path));
            char *fname2;
            char *brk;
            fname2 = strdup(fname);
            brk = strstr(fname2,"/gen/");
            brk[0] = '\0';
            brk = strrchr(fname2,'/');
            fprintf(genc,"#include \"..%s/gen/%s\"\n",brk,basename(fname));
            free(fname2);
            targets[ntargets] = hdata[i].targets[l].path;
            ntargets++;
          }

          p[1] = 'h';
          
          switch (hdata[i].targets[l].type) {
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
            case FR_JSON: 
            sprintf(format,"dejson/func");
            sprintf(format2,"dejson/proto");
            break;
            case FR_EBML: 
            sprintf(format,"debml/func");
            sprintf(format2,"debml/proto");
            break;
            case HELPERS: 
            sprintf(format,"helper");
            sprintf(format2,"helper_proto");
            codegen(hdata[i].defs,hdata[i].def_count,prefix,suffix,"enum_utils",out);
            break;
            default: break;
          }

          codegen(hdata[i].defs,hdata[i].def_count,prefix,suffix,format,out);
          codegen(hdata[i].defs,hdata[i].def_count,prefix,suffix,format2,header);

          fclose(out);
          fclose(header);
        }
      }
    }
  }

  return;
}

int main(int argc, char *argv[]) {

  header_data *hdata;
  int n;
  int i;
  char *rpath = NULL;
  FILE *genc;
  char gencpath[256];

  if (argc != 5) {
    print_usage(argv[0]);
    return 1;
  }

  hdata = calloc(MAX_HEADERS,sizeof(header_data));

  for (i = 0; i < MAX_HEADERS; i++) {
    hdata[i].defs = calloc(MAX_HEADER_DEFS,sizeof(struct_data));
    hdata[i].targets = calloc(MAX_TARGETS,sizeof(cgen_target));
  }

  n = gather_struct_definitions(hdata,"kr",argv[1]);

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

  common_gen(hdata,n,argv[2],argv[3],rpath,genc);

  type_common_gen(hdata,n,argv[2],argv[3],rpath);

  files_gen(hdata,n,argv[2],argv[3],genc);

  fprintf(genc,"#include \"ebml_common.c\"\n");
  fprintf(genc,"#include \"text_common.c\"\n");
  fprintf(genc,"#include \"debml_common.c\"\n");
  fprintf(genc,"#include \"json_common.c\"\n");
  fprintf(genc,"#include \"jsmn/jsmn.c\"\n");
  fprintf(genc,"#include \"dejson_common.c\"\n\n");

  for (i = 0; i < MAX_HEADERS; i++) {
    free(hdata[i].defs);
    free(hdata[i].targets);
  }

  free(hdata);
  fclose(genc);

  return 0;
}