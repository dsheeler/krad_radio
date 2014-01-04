#include "structs_gathering.h"

static int32_t clean_string (char *input, char *output) {

  uint32_t i;
  uint32_t j;
  uint8_t stop;

  i = 0;
  j = 0;
  stop = 0;

  while (input[i] != '\0') {
    if (!isspace(input[i])) {
      output[j] = input[i];
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

  output[j] = '\0';

  return j;
}

static void get_memb_data_info (char *line, 
  member_info *memb) {

  char *start;
  char *end;
  char *info[4];
  int i = 0;

  start = strstr(line,"/*");
  end = strstr(line,"*/");
  memset(info,0,sizeof(info));

  if ((memb->type == T_CHAR) && memb->arr) {
    memb->type_info.char_info.max = memb->arr - 1;
  }

  if (start && end) {
    info[0] = &start[2];
    end[0] = '\0';

    if ((memb->type == T_CHAR) && memb->arr) {
      if (strstr(info[0],"not null")) {
        memb->type_info.char_info.notnull = 1;
      }
      return;
    }

    strtod(info[0],&info[1]);

    if (info[0] == info[1]) {
      info[0] = NULL;
    } else {
      info[1]++;
      i++;
    }

    strtod(info[1],&info[2]);

    if (info[1] == info[2]) {
      info[1] = NULL;
    } else {
      info[2]++;
      i++;
    }

    strtod(info[2],&info[3]);

    if (info[2] == info[3]) {
      info[2] = NULL;
    } else {
      i++;
    }

    end[0] = '*';
  }

  switch(memb->type) {
    case T_INT32: {
      if (i == 3) {
        memb->type_info.int32_info.init = atoi(info[0]);
        memb->type_info.int32_info.min = atoi(info[1]);
        memb->type_info.int32_info.max = atoi(info[2]);
      } else if (i == 2) {
        memb->type_info.int32_info.min = atoi(info[0]);
        memb->type_info.int32_info.max = atoi(info[1]);
      }
      break;
    }
    case T_INT64: {
      if (i == 3) {
        memb->type_info.int64_info.init = atol(info[0]);
        memb->type_info.int64_info.min = atol(info[1]);
        memb->type_info.int64_info.max = atol(info[2]);
      } else if (i == 2) {
        memb->type_info.int64_info.min = atol(info[0]);
        memb->type_info.int64_info.max = atol(info[1]);
      }
      break;
    }
    case T_UINT32: {
      if (i == 3) {
        memb->type_info.uint32_info.init = atoi(info[0]);
        memb->type_info.uint32_info.min = atoi(info[1]);
        memb->type_info.uint32_info.max = atoi(info[2]);
      } else if (i == 2) {
        memb->type_info.uint32_info.min = atoi(info[0]);
        memb->type_info.uint32_info.max = atoi(info[1]);
      }
      break;
    }
    case T_UINT64: {
      if (i == 3) {
        memb->type_info.uint64_info.init = atol(info[0]);
        memb->type_info.uint64_info.min = atol(info[1]);
        memb->type_info.uint64_info.max = atol(info[2]);
      } else if (i == 2) {
        memb->type_info.uint64_info.min = atol(info[0]);
        memb->type_info.uint64_info.max = atol(info[1]);
      }
      break;
    }
    case T_FLOAT: {
      if (i == 3) {
        memb->type_info.float_info.init = atof(info[0]);
        memb->type_info.float_info.min = atof(info[1]);
        memb->type_info.float_info.max = atof(info[2]);
      } else if (i == 2) {
        memb->type_info.float_info.min = atof(info[0]);
        memb->type_info.float_info.max = atof(info[1]);
      }
      break;
    }
    case T_DOUBLE: {
      if (i == 3) {
        memb->type_info.double_info.init = atof(info[0]);
        memb->type_info.double_info.min = atof(info[1]);
        memb->type_info.double_info.max = atof(info[2]);
      } else if (i == 2) {
        memb->type_info.double_info.min = atof(info[0]);
        memb->type_info.double_info.max = atof(info[1]);
      }
      break;
    }
    default: break;
  }

  return;
}

member_type str_to_member_type(char *str) {
  if (!strncmp(str,"int32_t",7) || !strcmp(str,"int")) {
    return T_INT32;
  } else if (!strncmp(str,"int64_t",7)) {
    return T_INT64;
  } else if (!strncmp(str,"char",4)) {
    return T_CHAR;
  } else if (!strncmp(str,"uint32_t",8) || !strncmp(str,"uint16_t",8) || !strcmp(str,"uint")) {
    return T_UINT32;
  } else if (!strncmp(str,"uint64_t",8)) {
    return T_UINT64;
  } else if (!strncmp(str,"float",5)) {
    return T_FLOAT;
  } else if (!strncmp(str,"double",6)) {
    return T_DOUBLE;
  } else {
    return T_STRUCT;
  }
}

char *member_type_to_str(member_type type) {
  switch (type) {
    case T_CHAR: {
      return "char";
    }
    case T_INT32: {
      return "int32_t";
    }
    case T_INT64: {
      return "int64_t";
    }
    case T_UINT32: {
      return "uint32_t";
    }
    case T_UINT64: {
      return "uint64_t";
    }
    case T_FLOAT: {
      return "float";
    }
    case T_DOUBLE: {
      return "double";
    }
    default: return "";
  }
}

static void get_memb_name_and_type(char *line, 
  member_info *memb) {

  char *p;
  char *p2;
  char *back;
  char type_name[64];
  char clean[strlen(line)];
  clean_string(line,clean);

  snprintf(type_name,64,"%s",clean);
  memb->type = str_to_member_type(clean);

  if (memb->type == T_STRUCT) {
    snprintf(memb->type_info.substruct_info.type_name,NAME_MAX_LEN,"%s",clean);
  }

  p = strchr(line,';');
  p[0] = '\0';
  back = &p[1];

  if ((p = strrchr(line,'*'))) {
    /* it's a pointer */
    p2 = p;
    while (*p2 == '*') {
      memb->ptr++;
      p2--;
    }
    char clean[strlen(&p[1])];
    clean_string(&p[1],clean);
    snprintf(memb->name,NAME_MAX_LEN,"%s",clean);
  } else {
    p = strstr(line,type_name);
    if (p) {
      char clean[sizeof(type_name)];
      clean_string(&p[strlen(type_name)],clean);

      if ( (p = strchr(clean,'[')) ) {
        memb->arr = strtod(&p[1],NULL);
        if (!memb->arr) {
          char *p2;
          p2 = strchr(p,']');
          p2[0] = '\0';
          snprintf(memb->len_def,32,"%s",&p[1]);
        }
        p[0] = '\0';
      } 
      snprintf(memb->name,NAME_MAX_LEN,"%s",clean);
    }
  }

  get_memb_data_info(back,memb);

  return;
}

static int get_enum_memb(char *line, 
  member_info *memb) {

  char *p;
  char clean[strlen(line)];

  p = strchr(line,',');

  if (p) {
    p[0] = '\0';
    clean_string(line,clean);
    snprintf(memb->name,NAME_MAX_LEN,"%s",clean);
    memb->type = T_UINT32;
    p[0] = ',';
    return 0;
  } else {
    clean_string(line,clean);
    snprintf(memb->name,NAME_MAX_LEN,"%s",clean);
    memb->type = T_UINT32;
    return 1;
  }

}

static uint32_t is_dir(const char *path) {
  struct stat buf;
  char rpath[PATH_MAX];
  char *res;
  res = realpath(path,rpath);
  if (res) {
    stat(rpath, &buf);
    return S_ISDIR(buf.st_mode);
  } else {
    return 0;
  }
}

static int32_t header_files_or_subdir_select(const struct direct *entry) {

  if (!strncmp(&entry->d_name[strlen(entry->d_name)-2],".h",2)) {
    return 1;
  } else if (is_dir(entry->d_name) && strcmp(entry->d_name,".") && strcmp(entry->d_name,"..")) {
    return 1;
  } else {
    return 0;
  }

}

static int scandir_recursive(char *path, int *num, header_data *hdata) {
  
  char cwd[256];
  char rpath[PATH_MAX];
  int dir_count;
  struct dirent **dirs;
  char *res;
  int i;
  //static int depth = 0;

  if (!is_dir(path)) {
    // special case, we have just an header file
  } else {

    getcwd(cwd,sizeof(cwd));
    chdir(path);

    dir_count = scandir(".", &dirs, header_files_or_subdir_select, alphasort);

    if (dir_count < 0) {
      return -1;
    }

    for (i = 0; i < dir_count; i++) {

      if (is_dir(dirs[i]->d_name)) {
       // depth++;
        //if (depth > 1) {
          scandir_recursive(dirs[i]->d_name,num,hdata);
        //}
      } else {
        if (*num < MAX_HEADERS) {
          res = realpath(dirs[i]->d_name,rpath);
          snprintf(hdata[*num].path,PATH_MAX_LEN,"%s",rpath);
          if (res)
            (*num)++;
        } else {
          return 0;
        }
      }

    }
    for (i = 0; i < dir_count; i++) {
      free(dirs[i]);
    }

    if (dirs) {
      free(dirs);
    }
    chdir(cwd);
  }

  return 0;

}

static int check_for_cgen_target(char *line, char *hpath, cgen_target *target) {

  char *p;
  char *p2;
  char *fname;
  char *target_str;
  char *res;

  p = strstr(line,"#include \"");

  if (p) {
    p = strstr(p,"gen/");
  }

  if (p) {
    p2 = strrchr(p,'"');
    if (p2) {
      p2[0] = '\0';
      fname = strchr(line,'"');
      if (fname) {
        fname++;
        if ( (target_str = strstr(fname,"to_ebml")) ) {
          target->type = TO_EBML;
        } else if ( (target_str = strstr(fname,"to_json")) ) {
          target->type = TO_JSON;
        } else if ( (target_str = strstr(fname,"from_json")) ) {
          target->type = FR_JSON;
        } else if ( (target_str = strstr(fname,"to_text")) ) {
          target->type = TO_TEXT;
        } else if ( (target_str = strstr(fname,"from_ebml")) ) {
          target->type = FR_EBML;
        } else if ( (target_str = strstr(fname,"helpers")) ) {
          target->type = HELPERS;
        } else if ( (target_str = strstr(fname,"config")) ) {
          target->type = CONFIG;
        }

        res = strrchr(hpath,'/');
        if (res) {
          res[0] = '\0';
          snprintf(target->path,PATH_MAX_LEN,"%s/%s",hpath,fname);
          res[0] = '/';
          return 1;
        }

      }
    }
  }

  return 0;
}

static int defs_from_header(header_data *hdata) {

  FILE *fp;
  struct_data *defs = hdata->defs;
  cgen_target *targets = hdata->targets;
  char *line = NULL;
  size_t len = 0;
  int index = 0;
  int read;
  int linen;
  int lenacc;
  char *p1;
  char *p2;
  char *p3;
  int inside_struct = 0;
  int in_substruct = 0;
  int in_enum = 0;
  int in_comment = 0;
  int substruct = 0;
  int newdefs = 0;

  fp = fopen(hdata->path,"r");

  if (fp == NULL) {
    return index;
  }

  linen = 1;
  lenacc = 0;

  while ((read = getline(&line,&len,fp)) != -1) {

    if (in_comment) {
      if (strstr(line,"*/")) {
        in_comment = 0;
      }
      continue;
    } else {
      if (!strncmp(line,"/*",2)) {
        in_comment = 1;
        if (strstr(line,"*/")) {
          in_comment = 0;
        }
        continue;
      }
    }

    /* Checking for targets */
    if (hdata->target_count >= MAX_TARGETS) {
      fprintf(stderr,"MAX_TARGETS exceeded!\n");
      exit(-1);
    }

    if (check_for_cgen_target(line,hdata->path,&targets[hdata->target_count])) {
      hdata->target_count++;
      continue;
    }
      
    if (index >= MAX_HEADER_DEFS) {
      fprintf(stderr,"MAX_HEADER_DEFS exceeded\n");
      exit(1);
    }

    if (inside_struct) {

      p3 = strchr(line,'}');

      if (p3) {
        if (strchr(defs[index].info.name,'{') || in_substruct) {
          char *end;
          end = strchr(p3,';');
          char sname[end - p3];
          end[0] = '\0';
          clean_string(&p3[1],sname);
          if (in_substruct) {
            //defs[index-1].members_info[defs[index-1].members].name = strdup(sname);
            //defs[index-1].members_info[defs[index-1].members].type = strdup(defs[index].info.name);
            //defs[index-1].members_info[defs[index-1].members].sub = &defs[index];
            //defs[index-1].members++;
          } else {
            snprintf(defs[index].info.name,NAME_MAX_LEN,"%s",sname);
            defs[index].info.is_typedef = 1;
          }
        }
           
        if (!in_substruct) {
          index++;
          index += substruct;
          lenacc = 0;
          newdefs++;
          inside_struct = 0;
          in_enum = 0;
          substruct = 0;
        } else {
          in_substruct = 0;
          substruct++;
          index--;
          newdefs++;
        }
        
      } else {
        if (strchr(line,';')) {
          if ((lenacc+strlen(line)) < DEF_MAX_LEN) {
            strcpy(&(defs[index].definition[lenacc]),line);
            lenacc += strlen(line);
            if (defs[index].info.member_count >= MEMBER_MAX) {
              fprintf(stderr,"MEMBER_MAX exceeded in %s\n",defs[index].filename);
              exit(1);
            }
            get_memb_name_and_type(line,&defs[index].info.members[defs[index].info.member_count]);
            defs[index].info.member_count++;
          } else {
            fprintf(stderr,"DEF_MAX_LEN exceeded\n");
            exit(1);
          }
        } else if (in_enum) {

          if (defs[index].info.member_count >= MEMBER_MAX) {
            fprintf(stderr,"MEMBER_MAX exceeded in %s\n",defs[index].filename);
            exit(1);
          }
          // enum case
          if (!strchr(line,'#')) {
            if (get_enum_memb(line,&defs[index].info.members[defs[index].info.member_count])) {
              in_enum = 0;
            }
            defs[index].info.member_count++;
          }

        } else {
          /* TODO: handle the case where there is a struct/union definition inside another struct definition 
          if (strstr(line,"union") || strstr(line,"struct")) {
            in_substruct = 1;
            asprintf(&(defs[index+1].name),"%s_sub",defs[index].info.name);
            defs[index+1].fullpath = strdup(header_path);
            defs[index+1].line = linen;
            defs[index+1].issub = 1;
            if (strstr(line,"union")) {
             defs[index+1].isunion = 1; 
            }
            index++;
          }
          */
        }
      }

    } else {

      p1 = strstr(line,"struct");

      if (p1) {
        defs[index].info.type = ST_STRUCT;
      }

      if (!p1) {
        p1 = strstr(line,"union");
        if (p1) {
          defs[index].info.type = ST_UNION;
        }
      }

      if (!p1) {
        p1 = strstr(line,"enum");
        if (p1) {
          defs[index].info.type = ST_ENUM;
          in_enum = 1;
        }
      }

      if (p1) {
        p2 = strchr(p1,'{');
        if (p2) {
          char sname[p2-p1];

          if (in_enum) {
            clean_string(&p1[4],sname);
          } else if (defs[index].info.type == ST_UNION) {
            clean_string(&p1[5],sname);
          } else {
            clean_string(&p1[6],sname);
          }
          
          snprintf(defs[index].info.name,NAME_MAX_LEN,"%s",sname);
          snprintf(defs[index].filename,FILENAME_MAX_LEN,"%s",strrchr(hdata->path,'/')+1);
          defs[index].line_number = linen;

          inside_struct = 1;         
        }
      }
    }

    linen++;
  }

  if (line) {
    free(line);
  }

  return newdefs;
}

int gather_struct_definitions(header_data *hdata, char *fprefix, char *path) {
  int i;
  int num;
  char *p;

  num = 0;
  
  scandir_recursive(path,&num,hdata);

  if (num > MAX_HEADERS) {
    fprintf(stderr, "MAX_HEADERS exceeded!\n");
    exit(-1);
  }

  for (i = 0; i < num; i++) {
    p = strrchr(hdata[i].path,'/');
    if (p) {
      if (fprefix) {
        if (!strncmp(fprefix,&p[1],strlen(fprefix))) {          
          hdata[i].def_count = defs_from_header(&hdata[i]);
        }
      } else {
        hdata[i].def_count = defs_from_header(&hdata[i]);
      }
    }
  } 

  return num;
}

int is_prefix (const char *str, const char *prefix) {
  if (!prefix[0]) {
    return 1;
  }
  return !strncmp(str,prefix,strlen(prefix));
}

int is_suffix (const char *str, const char *suffix) {
  int len1;
  int len2;

  if (!suffix[0]) {
    return 1;
  }

  len1 = strlen(str);
  len2 = strlen(suffix);

  if (len1 >= len2) {
    return !strncmp(&str[len1-len2],suffix,len2);
  }

  return 0;
}

static void print_memb_data_info(member_info *memb) {


  switch (memb->type) {
    case T_CHAR: {
      if (memb->arr) {
        printf("    max size %d\n",memb->type_info.char_info.max);
        printf("    %s\n",memb->type_info.char_info.notnull ? "can't be null" : "can be null");
      }
      break;
    }
    case T_INT32: {
      printf("    default value %d\n",memb->type_info.int32_info.init);      
      printf("    min value %d\n",memb->type_info.int32_info.min);
      printf("    max value %d\n",memb->type_info.int32_info.max);
      break;
    }
    case T_INT64: {
      printf("    default value %" PRId64 "\n",memb->type_info.int64_info.init);      
      printf("    min value %" PRId64 "\n",memb->type_info.int64_info.min);
      printf("    max value %" PRId64 "\n",memb->type_info.int64_info.max);
      break;
    }
    case T_UINT32: {
      printf("    default value %u\n",memb->type_info.uint32_info.init);      
      printf("    min value %u\n",memb->type_info.uint32_info.min);
      printf("    max value %u\n",memb->type_info.uint32_info.max);
      break;
    }
    case T_UINT64: {
      printf("    default value %" PRIu64 "\n",memb->type_info.uint64_info.init);      
      printf("    min value %" PRIu64 "\n",memb->type_info.uint64_info.min);
      printf("    max value %" PRIu64 "\n",memb->type_info.uint64_info.max);
      break;
    }
    case T_FLOAT: {
      printf("    default value %0.2f\n",memb->type_info.float_info.init);      
      printf("    min value %0.2f\n",memb->type_info.float_info.min);
      printf("    max value %0.2f\n",memb->type_info.float_info.max);
      break;
    }
    case T_DOUBLE: {
      printf("    default value %0.2f\n",memb->type_info.double_info.init);      
      printf("    min value %0.2f\n",memb->type_info.double_info.min);
      printf("    max value %0.2f\n",memb->type_info.double_info.max);
      break;
    }
    default : break;
  }

  return;
}

int print_structs_defs(header_data *hdata, char *prefix,
 char *suffix, char *format) {

  int i;
  int j;
  const char *type;

  if (!strncmp(format,"names",5)) {
    for (i = 0; i < hdata->def_count; i++) {
      if (is_prefix(hdata->defs[i].info.name,prefix) && is_suffix(hdata->defs[i].info.name,suffix)) {
        if (hdata->defs[i].info.name[0])
          printf("%s\n",hdata->defs[i].info.name);
      }
    }
    return 0;
  }

  if (!strncmp(format,"header",5)) {
    for (i  = 0; i < hdata->def_count; i++) {
      if (is_prefix(hdata->defs[i].info.name,prefix) && is_suffix(hdata->defs[i].info.name,suffix)) {
        if (hdata->defs[i].info.type == ST_UNION) {
          printf("union %s {\n",hdata->defs[i].info.name);
          printf("%s",hdata->defs[i].definition);
          printf("};\n");
        } else if (hdata->defs[i].info.type == ST_STRUCT) {
          printf("struct %s {\n",hdata->defs[i].info.name);
          printf("%s",hdata->defs[i].definition);
          printf("};\n");
        }
      }
    }
    return 0;
  }

  if (!strncmp(format,"info",4)) {
    for (i  = 0; i < hdata->def_count; i++) {
      if (is_prefix(hdata->defs[i].info.name,prefix) && is_suffix(hdata->defs[i].info.name,suffix)) {
        printf("Name: %s\n",hdata->defs[i].info.name);
        printf("Path: %s\n",hdata->defs[i].filename);
        printf("Line: %d\n",hdata->defs[i].line_number);
        printf("Members No: %d\n\n",hdata->defs[i].info.member_count);
      }
    }
    return 0;
  }

  if (!strncmp(format,"targets",7)) {

    if (hdata->target_count) {
      printf("%s has following targets:\n",hdata->path);
    }

    for (j = 0; j < hdata->target_count; j++) {
      if (hdata->targets[j].type) {

        switch (hdata->targets[j].type) {
          case TO_EBML:
          type = "TO_EBML";
          break;
          case TO_TEXT: 
          type = "TO_TEXT";
          break;
          case TO_JSON: 
          type = "TO_JSON";
          break;
          case FR_JSON: 
          type = "FR_JSON";
          break;
          case FR_EBML: 
          type = "FR_EBML";
          break;
          case HELPERS: 
          type = "HELPERS";
          break;
          case CONFIG:
          type = "CONFIG";
          break;
          default: 
          type = "NONE"; 
          break;
        }

        printf("    %s of type %s\n",hdata->targets[j].path,type);
      }
    }
    return 0;
  }

  if (!strncmp(format,"members",7)) {
    for (i = 0; i < hdata->def_count; i++) {
      if (is_prefix(hdata->defs[i].info.name,prefix) && is_suffix(hdata->defs[i].info.name,suffix)) {

        if (hdata->defs[i].info.type == ST_UNION)
          printf("Union Name: %s\n",hdata->defs[i].info.name);
        else if (hdata->defs[i].info.type == ST_ENUM)
          printf("Enum Name: %s\n",hdata->defs[i].info.name);
        else 
          printf("Struct Name: %s\n",hdata->defs[i].info.name);
        
        for (j = 0; j < hdata->defs[i].info.member_count; j++) {
          printf("  Member Name: %s\n",hdata->defs[i].info.members[j].name);
          if (hdata->defs[i].info.members[j].type == T_STRUCT) {
            printf("  Member Type: %s\n",hdata->defs[i].info.members[j].type_info.substruct_info.type_name);
          } else {
            printf("  Member Type: %s\n",member_type_to_str(hdata->defs[i].info.members[j].type));
          }
          if (hdata->defs[i].info.members[j].arr || hdata->defs[i].info.members[j].len_def[0]) {
            if (hdata->defs[i].info.members[j].arr)
              printf("  Array of %d.\n",hdata->defs[i].info.members[j].arr);
            else
              printf("  Array of %s.\n",hdata->defs[i].info.members[j].len_def);
          }
          if (hdata->defs[i].info.members[j].ptr) {
            printf("  It's a pointer of level %d.\n",hdata->defs[i].info.members[j].ptr);
          }
/*          if (hdata->defs[i].info.members[j].type) {
            printf("  It's a sub");
            if (hdata->defs[i].members_info[j].sub->isunion) {
              printf("union\n");
            } else if (hdata->defs[i].members_info[j].sub->isenum) {
              printf("enum\n");
            } else {
              printf("struct\n");
            }
          }*/
          if (!strncmp(&format[7],"_data_info",10)) {
            print_memb_data_info(&(hdata->defs[i].info.members[j]));
          }
        }
      }
    }
    return 0;
  }

  return 1;
}
