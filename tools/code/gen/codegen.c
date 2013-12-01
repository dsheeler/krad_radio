#include "codegen_utils.h"

void print_usage(char *cmd) {
  printf("syntax:\n%s path prefix suffix type output\n",cmd);
  printf("Available types:\n");
  printf("ebml , json , text, cbor , sizeof, helper , common, type_common\n");
}

int main(int argc, char *argv[]) {

  struct struct_def defs[MAX_DEFS];
  int ndefs;
  FILE *out;
  FILE *header;
  char format[256];

  if (argc < 6) {
    print_usage(argv[0]);
    return 0;
  }

  memset(&defs,0,sizeof(defs));

  ndefs = gather_struct_definitions(defs,"krad",argv[1]);

  char hname[strlen(argv[5]+1)];
  strcpy(hname,argv[5]);
  hname[strlen(argv[5])-1] = 'h';
  out = fopen(argv[5],"w+");

  if (!out) {
    fprintf(stderr,"Invalid output file!\n");
    return 1;
  }

  if (!strncmp(argv[4],"helper",6)) {

    header = fopen(hname,"w+");

    if (!header) {
      fprintf(stderr,"Invalid output file!\n");
      return 1;
    }

    //printf("%d %s %s %s %s %s\n",ndefs,argv[1],argv[2],argv[3],argv[4],argv[5]);
    fprintf(out,"#include \"%s\"\n\n",basename(hname));
    codegen(defs,ndefs,argv[2],argv[3],argv[4],out);
    codegen(defs,ndefs,argv[2],argv[3],"enum_utils",out);
    fprintf(out,"\n");
    codegen(defs,ndefs,argv[2],argv[3],"helper_proto",header);
    fprintf(header,"\n");

    fclose(header);

  } else if (!strncmp(argv[4],"common",6)) {

    codegen(defs,ndefs,argv[2],argv[3],"includes",out);
    fprintf(out,"\n");
    fprintf(out,"\ntypedef struct {\n  int type;\n  void *actual;\n} uber_St;\n\n");
    codegen(defs,ndefs,argv[2],argv[3],"enum",out);
    fprintf(out,"\n\n");
    sprintf(format,"json/proto");
    codegen(defs,ndefs,argv[2],argv[3],format,out);
    fprintf(out,"\n");
    codegen(defs,ndefs,argv[2],argv[3],"helper_proto",out);
    fprintf(out,"\n");

  } else if (!strncmp(argv[4],"type_common",11)) {

    sprintf(format,"%s/typedef",argv[6]);
    codegen(defs,ndefs,argv[2],argv[3],format,out);
    fprintf(out,"\n");
    sprintf(format,"%s/array_func",argv[6]);
    codegen(defs,ndefs,argv[2],argv[3],format,out);
    fprintf(out,"\n");

  } else if (!strncmp(argv[4],"sizeof",6)) {

    codegen(defs,ndefs,argv[2],argv[3],argv[4],out);
    fprintf(out,"\n");

  } else if (!strncmp(argv[4],"jschema",7)) {

    codegen(defs,ndefs,argv[2],argv[3],argv[4],out);
    fprintf(out,"\n");

  } else {
    header = fopen(hname,"w+");

    if (!header) {
      fprintf(stderr,"Invalid output file!\n");
      return 1;
    }

    fprintf(header,"#include <stdio.h>\n");
    codegen(defs,ndefs,argv[2],argv[3],"includes",header);

    fprintf(out,"#include \"%s\"\n",basename(hname));

    sprintf(format,"%s/proto",argv[4]);
    codegen(defs,ndefs,argv[2],argv[3],format,header);
    fprintf(out,"\n");
    sprintf(format,"%s/func",argv[4]);
    codegen(defs,ndefs,argv[2],argv[3],format,out);
    fprintf(out,"\n");
    fclose(header);
  }

  fclose(out);

  return 0;
}
