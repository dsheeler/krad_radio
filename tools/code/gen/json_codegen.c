#include "json_codegen.h"

static void member_type_to_fmt(uint32_t type, char *str) {

  switch(type) {
    case KR_DY_INT32: strcpy(str,"%d"); break;
    case KR_DY_FLOAT: strcpy(str,"%0.2f"); break;
    case KR_DY_UINT32:  strcpy(str,"%u"); break;
    case KR_DY_UINT64:  strcpy(str,"%llu"); break;
    case KR_DY_CHAR: strcpy(str,"%c"); break;
    case KR_DY_ENUM:  strcpy(str,"%u"); break;
    case KR_DY_STRING: strcpy(str,"\\\"%s\\\""); break;
    default: break;
  }

  return;
}


static void dystruct_var_list_dump_recursive(dynamic_struct *dystruct, char *var_list, dynamic_struct *dystructs, uint32_t n, char *tmp) {
  uint32_t j;
  uint32_t i;
  dynamic_struct *substruct;

  for (j=0;j<dystruct->members;j++) {
    if (dystruct->dynamic_members[j].struct_name[0]) {
      substruct = get_struct_by_name(dystruct->dynamic_members[j].struct_name,dystructs,n);
      strcat(tmp,dystruct->dynamic_members[j].name);
      strcat(tmp,".");
      //dystruct_var_list_dump_recursive(substruct,var_list,dystructs,n,tmp);
    } else {
      strcat(var_list,",");
      strcat(var_list,"infostruct->");
      strcat(var_list,tmp);
      strcat(var_list,dystruct->dynamic_members[j].name);
    }
  }
  memset(tmp,0,256);
  return;
}

static void dystruct_var_list_dump(dynamic_struct *dystruct, dynamic_struct *dystructs, uint32_t nstructs, char *var_list) {
  char tmp[256];
  memset(tmp,0,256);
  dystruct_var_list_dump_recursive(dystruct,var_list,dystructs,nstructs,tmp);
  return;
}

static void codegen_prototype(dynamic_struct *dystruct, code_gen_opts *opts) {
  opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"int32_t %s_to_json (char *output, %s *infostruct, uint32_t max_len);\n",dystruct->name,dystruct->name);
  return;
} 

static void codegen_pre_fun(dynamic_struct *dystruct, code_gen_opts *opts) {
  opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"\nint32_t %s_to_json (char *output, %s *infostruct, uint32_t max_len) {\n  int32_t len; \n\n  len = snprintf(output,max_len,\"",dystruct->name,dystruct->name);
  return;
}

static void codegen_post_fun(dynamic_struct *dystruct, code_gen_opts *opts) {
  char var_list[MAX_BUFFER];
  memset(var_list,0,MAX_BUFFER);
  dystruct_var_list_dump(dystruct,opts->dystructs,opts->nstructs,var_list);
  opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"\",%s); \n\n  if (len>max_len) {\n    return 0;\n  } else {\n    return len;\n  } \n\n}\n",var_list+1);
  return;
}

static int32_t json_serializer_codegen(code_gen_opts *opts, dynamic_member *dymemb, uint8_t last_item) {

  uint32_t i;
  dynamic_struct *substruct;
  char type_fmt[8];

  // eventually I'll need to deal with arrays

  if (dymemb->struct_name[0]) {
    opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"\\\"%s\\\": {",dymemb->name);
    substruct = get_struct_by_name(dymemb->struct_name,opts->dystructs,opts->nstructs);
    if (substruct == NULL) {
      return -1;
    }
    for (i=0;i<substruct->members;i++) {
      if (i<substruct->members-1) {
        json_serializer_codegen(opts,&substruct->dynamic_members[i],0);
      } else {
        json_serializer_codegen(opts,&substruct->dynamic_members[i],1);
      }
    }
    opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"}");
    if (!last_item) {
      opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,",");
    }
  } else {
      member_type_to_fmt(dymemb->type,type_fmt);      
      opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"\\\"%s\\\": %s",dymemb->name,type_fmt);
      if (!last_item) {
        opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,",");
      }
  }

  return opts->buff_pos;
}

static int32_t codegen(code_gen_opts *opts) {

  uint32_t i;
  uint32_t j;

  dynamic_struct *dystructs;
  uint32_t n;

  dystructs = opts->dystructs;
  n = opts->nstructs;

  for (i=0;i<n;i++) {
    codegen_prototype(&dystructs[i],opts);
  }

  for (i=0;i<n;i++) {
      codegen_pre_fun(&dystructs[i],opts);
      for (j=0;j<dystructs[i].members;j++) {
        if (j<dystructs[i].members-1) {
          json_serializer_codegen(opts,&(dystructs[i].dynamic_members[j]),0);
        } else {
          json_serializer_codegen(opts,&(dystructs[i].dynamic_members[j]),1);
        }
      }
      
      codegen_post_fun(&dystructs[i],opts);
  }

  return 0;
}

int32_t main(int32_t argc, char *argv[]) {

  char *basepath;
  char *ofile;
  dynamic_struct dystructs[MAX_DYNAMIC_STRUCTS];
  code_gen_opts cgen_opts;
  int32_t res;
  int32_t nstructs;
  FILE *fd;

  char buff[MAX_BUFFER];

  if (argc != 3) {
    printf("Need 2 args: libpath and output file\n");
    return -1;
  } else {
    basepath = argv[1];
    ofile = argv[2];
  }

  memset(dystructs,0,sizeof(dynamic_struct)*MAX_DYNAMIC_STRUCTS);
  memset(&cgen_opts,0,sizeof(code_gen_opts));

  nstructs = parse_and_init(basepath,dystructs);

  if (nstructs<0) {
    fprintf(stderr,"Error: %s\n",strerror(errno));
    return -1;
  }

  cgen_opts.dystructs = dystructs;
  cgen_opts.nstructs = nstructs;
  cgen_opts.codegen_fun = codegen;
  cgen_opts.output_buffer = buff;
  cgen_opts.buff_size = sizeof(buff);

  res = cgen_opts.codegen_fun(&cgen_opts);

  if (res<0) {
    fprintf(stderr,"Error: %s\n",strerror(errno));
    return -1;
  }

  fd = fopen(ofile, "a+");
  if (!fd) {
    fprintf(stderr,"Error: %s\n",strerror(errno));
    return -1;
  }

  fprintf(fd,"%s",buff);
  fclose(fd);

  return 0;
}

