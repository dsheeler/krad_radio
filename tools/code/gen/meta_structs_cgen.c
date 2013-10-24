#include "meta_structs_cgen.h"

static void member_type_to_string(uint32_t type, char *str) {

  switch (type) {
    case KR_DY_INT32: strcpy(str,"KR_META_INT32"); break;
    case KR_DY_FLOAT: strcpy(str,"KR_META_FLOAT"); break;
    case KR_DY_UINT32:  strcpy(str,"KR_META_UINT32"); break;
    case KR_DY_UINT64:  strcpy(str,"KR_META_UINT64"); break;
    case KR_DY_CHAR: strcpy(str,"KR_META_CHAR"); break;
    case KR_DY_ENUM:  strcpy(str,"KR_META_ENUM"); break;
    case KR_DY_STRING: strcpy(str,"KR_META_STRING"); break;
    case KR_DY_STRUCT: strcpy(str,"KR_META_STRUCT"); break;
    default: break;
  }

  return;
}

static void capitalize(char *str1, char *str2) {
  uint32_t len;
  uint32_t i;

  len = strlen(str1);

  for(i=0;i<len;i++) {
    str2[i] = toupper(str1[i]);
  }

  str2[len] = '\0';

  return;
}

static int32_t meta_structs_codegen(code_gen_opts *opts, 
  dynamic_member *dymemb, uint32_t idx, uint32_t struct_idx, uint8_t last) {

  char str[256];
  char cap[256];

  member_type_to_string(dymemb->type,str);
  memset(cap,0,256);

  if (!last) {
    if (dymemb->type == KR_DY_STRUCT) {
      capitalize(dymemb->struct_name,cap);
      opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"  .members[%d].name = \"%s\",\n  .members[%d].type = %s,\n  .members[%d].struct_type = %s,\n",idx,dymemb->name,idx,str,idx,cap);
    } else {
      opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"  .members[%d].name = \"%s\",\n  .members[%d].type = %s,\n  .members[%d].value_ptr = .info->%s.%s,\n",idx,dymemb->name,idx,str,idx,opts->dystructs[struct_idx].name,dymemb->name);
    }
  } else {
    if (dymemb->type == KR_DY_STRUCT) {
      capitalize(dymemb->struct_name,cap);
      opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"  .members[%d].name = \"%s\",\n  .members[%d].type = %s,\n  .members[%d].struct_type = %s\n",idx,dymemb->name,idx,str,idx,cap);
    } else {
      opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"  .members[%d].name = \"%s\",\n  .members[%d].type = %s,\n  .members[%d].value_ptr = .info->%s.%s\n",idx,dymemb->name,idx,str,idx,opts->dystructs[struct_idx].name,dymemb->name);
    }
  }

  return opts->buff_pos;
}

static int32_t codegen(code_gen_opts *opts) {

  uint32_t i;
  uint32_t j;
  char str_upper[256];
  dynamic_struct *dystructs;
  cgen_limits *limits;
  uint32_t n;
  uint32_t name_len;

  dystructs = opts->dystructs;
  limits = &opts->limits;
  n = opts->nstructs;

  memset(str_upper,0,256);
  memset(limits,0,sizeof(cgen_limits));

  for (i=0;i<n;i++) {
    if (dystructs[i].members > limits->memb_max) {
      limits->memb_max = dystructs[i].members;
    }
    name_len = strlen(dystructs[i].name)+1;
    if (name_len > limits->struct_name_max) {
      limits->struct_name_max = name_len;
    }
    for (j=0;j<dystructs[i].members;j++) {
      name_len = strlen(dystructs[i].dynamic_members[j].name)+1;
      if (name_len > limits->memb_name_max) {
        limits->memb_name_max = name_len;
      }
    }
  }

  opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"#define KR_INFO_TYPES_COUNT %d\n#define KR_META_INFO_MEMBERS_MAX %d\n#define KR_META_MEMB_NAME_MAX %d\n#define KR_META_STRUCT_NAME_MAX %d\n\n",n,limits->memb_max,limits->memb_name_max,limits->struct_name_max);

  opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"typedef struct kr_info kr_info;\ntypedef struct kr_meta_info kr_meta_info;\ntypedef struct kr_meta_info_member kr_meta_info_member;\n\ntypedef enum {\n");

  for (i=0;i<n;i++) {
    capitalize(dystructs[i].name,str_upper);
    if (i<n-1) {
      opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"  %s,\n",str_upper);
    } else {
      opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"  %s\n",str_upper);
    }
  }

  opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"} kr_info_type;\n\n");

  opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"struct kr_info {\n  kr_info_type type;\n  union {\n");

  for (i=0;i<n;i++) {
    opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"    %s %s;\n",dystructs[i].name,dystructs[i].name+3);
  }

  opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"  } info;\n};\n\n");

  opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"typedef enum {\n  KR_META_INT32,\n  KR_META_UINT32,\n  KR_META_UINT64,\n  KR_META_FLOAT,\n  KR_META_CHAR,\n  KR_META_STRING,\n  KR_META_STRUCT,\n  KR_META_ENUM \n} kr_member_type;\n\nstruct kr_meta_info_member {\n  char name[KR_META_MEMB_NAME_MAX]; \n  kr_member_type type;\n  kr_info_type struct_type;\n  char struct_name[KR_META_STRUCT_NAME_MAX];\n  union {\n    int *integer;\n    uint32_t *uint32;\n    int64_t *integer64;\n    float *real;\n    char *ch;\n    char *string[64];\n  } value_ptr; \n  uint8_t array;\n  uint32_t array_len;\n };\n\nstruct kr_meta_info {\n  char name[KR_META_STRUCT_NAME_MAX]; \n  uint32_t members_count; \n  kr_info_type type;\n  kr_info *info;\n  uint8_t sub; \n  kr_meta_info_member members[KR_META_INFO_MEMBERS_MAX];\n };\n\n");
  for (i=0;i<n;i++) {
      capitalize(dystructs[i].name,str_upper);
      opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"static const kr_meta_info %s_meta = {\n  .name = \"%s\", \n  .type = %s, \n  .members_count = %d,\n",dystructs[i].name,dystructs[i].name,str_upper,dystructs[i].members);
      for (j=0;j<dystructs[i].members;j++) {
        if (j < dystructs[i].members - 1) {
          meta_structs_codegen(opts,&(dystructs[i].dynamic_members[j]),j,i,0);
        } else {
          meta_structs_codegen(opts,&(dystructs[i].dynamic_members[j]),j,i,1);
        }
      }
      opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"};\n\n");
  }

  opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"static const kr_meta_info *kr_meta_info_structs[KR_INFO_TYPES_COUNT] = {\n");

  for (i=0;i<n;i++) {
    if (i < n-1) {
      opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"  &%s_meta,\n",dystructs[i].name);
    } else {
      opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"  &%s_meta\n",dystructs[i].name);
    }
  }

  opts->buff_pos += snprintf(opts->output_buffer+opts->buff_pos,opts->buff_size-opts->buff_pos,"};\n\n",n);
  
  return 0;
}

int32_t main (int32_t argc, char **argv) {

  int32_t nstructs;
  int32_t res;
  dynamic_struct dystructs[MAX_DYNAMIC_STRUCTS];
  char buffer[MAX_BUFFER];
  FILE *fd;
  code_gen_opts cgen_opts;

  if (argc != 3) {
    printf("Need 2 args: libpath and output file \n");
    return -1;
  }

  nstructs = parse_and_init(argv[1],dystructs);

  if (nstructs <= 0) {
    printf("Failed to read any info struct\n");
    return -1;
  }

  cgen_opts.dystructs = dystructs;
  cgen_opts.nstructs = nstructs;
  cgen_opts.output_buffer = buffer;
  cgen_opts.buff_size = sizeof(buffer);
  cgen_opts.codegen_fun = codegen;

  res = cgen_opts.codegen_fun(&cgen_opts);

  if (res<0) {
    fprintf(stderr,"Error: %s\n",strerror(errno));
    return -1;
  }
  
  fd = fopen(argv[2], "a+");
  if (!fd) {
    fprintf(stderr,"Error: %s\n",strerror(errno));
    return -1;
  }

  fprintf(fd,"%s",buffer);
  fclose(fd);

  return 0;
}
