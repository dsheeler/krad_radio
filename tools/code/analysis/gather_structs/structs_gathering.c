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
  struct struct_memb_def *memb) {

  char *start;
  char *end;
  char *p[2];
  int i = 0;

  start = strstr(line,"/*");
  end = strstr(line,"*/");

  if (start && end) {
    start = &start[2];
    end[0] = '\0';
    p[0] = start;
    p[1] = NULL;

    if (!strncmp(memb->type,"char",4)) {
      if (memb->array) {
        memb->data_info.info.int_info[0] = 0;
        memb->data_info.info.int_info[1] = 1;
        memb->data_info.info.int_info[2] = memb->array - 1;
        memb->data_info.type = MEMB_TYPE_STRING;
        end[0] = '*';
        return;
      }
    }

    do {

      if (!strncmp(memb->type,"int",3)) {
        memb->data_info.info.int_info[i] = strtod(p[0],&p[1]);
        memb->data_info.type = MEMB_TYPE_INT;
      } else if (!strncmp(memb->type,"float",5)) {
        memb->data_info.info.float_info[i] = strtof(p[0],&p[1]);
        memb->data_info.type = MEMB_TYPE_FLOAT;
      } else if (!strncmp(memb->type,"uint",4)) {
        memb->data_info.info.int_info[i] = strtod(p[0],&p[1]);
        memb->data_info.type = MEMB_TYPE_UINT;
      } else {
        strtod(p[0],&p[1]);
      }

      if (p[0] == p[1]) {
        break;
      }

      i++;

      p[0] = &p[1][1];
      p[1] = NULL;
    } while (1);

    end[0] = '*';
  }

  return;
}

static void get_memb_name_and_type(char *line, 
  struct struct_memb_def *memb) {

  char *p;
  char *p2;
  char *back;
  char clean[strlen(line)];
  clean_string(line,clean);
  asprintf(&(memb->type),"%s",clean);

  p = strchr(line,';');
  p[0] = '\0';
  back = &p[1];

  if ((p = strrchr(line,'*'))) {
    /* it's a pointer */
    p2 = p;
    while (*p2 == '*') {
      memb->pointer++;
      p2--;
    }
    char clean[strlen(&p[1])];
    clean_string(&p[1],clean);
    asprintf(&(memb->name),"%s",clean);
  } else {
    p = strstr(line,memb->type);
    if (p) {
      char clean[strlen(&p[strlen(memb->type)])];
      clean_string(&p[strlen(memb->type)],clean);

      if ( (p = strchr(clean,'[')) ) {
        memb->array = strtod(&p[1],NULL);
        p[0] = '\0';
      } 

      asprintf(&(memb->name),"%s",clean);
    }
  }

  get_memb_data_info(back,memb);

  return;
}

static int get_enum_memb(char *line, 
  struct struct_memb_def *memb) {

  char *p;
  char clean[strlen(line)];

  p = strchr(line,',');

  if (p) {
    p[0] = '\0';
    clean_string(line,clean);
    asprintf(&(memb->name),"%s",clean);
    asprintf(&(memb->type),"enum_val");
    p[0] = ',';
    return 0;
  } else {
    clean_string(line,clean);
    asprintf(&(memb->name),"%s",clean);
    asprintf(&(memb->type),"enum_val");
    return 1;
  }

}

static uint32_t is_dir(const char *path) {
  struct stat buf;
  char *rpath;
  rpath = realpath(path,NULL);
  if (rpath) {
    stat(rpath, &buf);
    free(rpath);
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

static int scandir_recursive(char *path, int *num, char **headers) {
  
  char cwd[256];
  int dir_count;
  struct dirent **dirs;
  char *rpath;
  int i;

  if (!is_dir(path)) {
    // special case, we have just an header file
  } else {

    getcwd(cwd,sizeof(cwd));
    chdir(path);

    dir_count = scandir(".", &dirs, header_files_or_subdir_select, alphasort);

    if (dir_count < 0) {
      return -1;
    }

    for (i=0;i<dir_count;i++) {

      if (is_dir(dirs[i]->d_name)) {
        scandir_recursive(dirs[i]->d_name,num,headers);
      } else {
        if (*num < MAX_HEADERS) {
          rpath = realpath(dirs[i]->d_name,NULL);
          asprintf(&headers[*num],"%s",rpath);
          free(rpath);
          (*num)++;
        } else {
          return 0;
        }
      }

    }

    chdir(cwd);
  }

  return 0;

}

static int defs_from_header(struct struct_def *defs, 
  char *header_path, int index) {

  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  int read;
  int linen;
  int lenacc;
  char *p1;
  char *p2;
  char *p3;
  int inside_struct = 0;
  int in_substruct = 0;
  int in_enum = 0;
  int substruct = 0;
  int newdefs = 0;

  fp = fopen(header_path,"r");

  if (fp == NULL) {
    return index;
  }

  linen = 1;
  lenacc = 0;

  while ((read = getline(&line,&len,fp)) != -1) {

    if (index >= MAX_DEFS) {
      fprintf(stderr,"MAX_DEFS exceeded\n");
      exit(1);
    }

    if (inside_struct) {

      p3 = strchr(line,'}');

      if (p3) {
        if (strchr(defs[index].name,'{') || in_substruct) {
          char *end;
          end = strchr(p3,';');
          char sname[end - p3];
          end[0] = '\0';
          clean_string(&p3[1],sname);
          if (in_substruct) {
            defs[index-1].members_info[defs[index-1].members].name = strdup(sname);
            defs[index-1].members_info[defs[index-1].members].type = strdup(defs[index].name);
            defs[index-1].members_info[defs[index-1].members].sub = &defs[index];
            defs[index-1].members++;
          } else {
            defs[index].name = strdup(sname);
            defs[index].istypedef = 1;
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
          if ((lenacc+strlen(line)) < MAX_DEF_LENGTH) {
            strcpy(&(defs[index].definition[lenacc]),line);
            lenacc += strlen(line);
            if (defs[index].members >= MAX_MEMB) {
              fprintf(stderr,"MAX_MEMB exceeded in %s\n",defs[index].name);
              exit(1);
            }
            get_memb_name_and_type(line,&defs[index].members_info[defs[index].members]);
            defs[index].members++;
          } else {
            fprintf(stderr,"MAX_DEF_LENGTH exceeded\n");
            exit(1);
          }
        } else if (in_enum) {

          if (defs[index].members >= MAX_MEMB) {
            fprintf(stderr,"MAX_MEMB exceeded in %s\n",defs[index].name);
            exit(1);
          }
          // enum case
          if (!strchr(line,'#')) {
            if (get_enum_memb(line,&defs[index].members_info[defs[index].members])) {
              in_enum = 0;
            }
            defs[index].members++;
          }

        } else {
          /* TODO: handle the case where there is a struct/union definition inside another struct definition */
          if (strstr(line,"union") || strstr(line,"struct")) {
            in_substruct = 1;
            asprintf(&(defs[index+1].name),"%s_sub",defs[index].name);
            defs[index+1].fullpath = strdup(header_path);
            defs[index+1].line = linen;
            defs[index+1].issub = 1;
            if (strstr(line,"union")) {
             defs[index+1].isunion = 1; 
            }
            index++;
          }
          
        }
      }

    } else {
      p1 = strstr(line,"struct");

      if (!p1) {
        p1 = strstr(line,"union");
        if (p1) {
          defs[index].isunion = 1;
        }
      }

      if (!p1) {
        p1 = strstr(line,"enum");
        if (p1) {
          defs[index].isenum = 1;
          in_enum = 1;
        }
      }

      if (p1) {
        p2 = strchr(p1,'{');
        if (p2) {
          char sname[p2-p1];

          if (in_enum) {
            clean_string(&p1[4],sname);
          } else if (defs[index].isunion) {
            clean_string(&p1[5],sname);
          } else {
            clean_string(&p1[6],sname);
          }
          
          defs[index].name = strdup(sname);
          defs[index].fullpath = strdup(header_path);
          defs[index].line = linen;
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

int gather_struct_definitions(struct struct_def *defs, char *fprefix, char *path) {
  int i;
  int num;
  int ndefs;
  char *headers[MAX_HEADERS];
  char *p;

  num = 0;
  ndefs = 0;
  
  scandir_recursive(path,&num,headers);

  for (i=0;i<num;i++) {
    if (fprefix) {
      p = strrchr(headers[i],'/');
      if (p) {
        if (!strncmp(fprefix,&p[1],strlen(fprefix))) {
          ndefs += defs_from_header(defs,headers[i],ndefs);
        }
      }
    }
  } 

  for (i=0;i<num;i++) {
    free(headers[i]);
  }

  return ndefs;
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

static void print_memb_data_info(struct struct_memb_def *memb) {

  switch (memb->data_info.type) {
    case MEMB_TYPE_UINT: {
      printf("    default value %u\n",memb->data_info.info.int_info[0]);
      printf("    min value %u\n",memb->data_info.info.int_info[1]);
      printf("    max value %u\n",memb->data_info.info.int_info[2]);
      break;
    }
    case MEMB_TYPE_INT:{
      printf("    default value %d\n",memb->data_info.info.int_info[0]);
      printf("    min value %d\n",memb->data_info.info.int_info[1]);
      printf("    max value %d\n",memb->data_info.info.int_info[2]);
      break;
    }
    case MEMB_TYPE_FLOAT:{
      printf("    default value %0.2f\n",memb->data_info.info.float_info[0]);
      printf("    min value %0.2f\n",memb->data_info.info.float_info[1]);
      printf("    max value %0.2f\n",memb->data_info.info.float_info[2]);
      break;
    }
    case MEMB_TYPE_STRING:
    case MEMB_TYPE_UNKNOWN:
    break;
  }

  return;
}

int print_structs_defs(struct struct_def *defs, 
  int ndefs, char *prefix, char *suffix, char *format) {

  int i;
  int j;

  if (!strncmp(format,"names",5)) {
    for (i=0;i<ndefs;i++) {
      if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix))
        printf("%s\n",defs[i].name);
    }
    return 0;
  }

  if (!strncmp(format,"header",5)) {
    for (i=0;i<ndefs;i++) {
      if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix)) {
        if (!defs[i].isunion) {
          printf("struct %s {\n",defs[i].name);
          printf("%s",defs[i].definition);
          printf("};\n");
        } else {
          printf("union %s {\n",defs[i].name);
          printf("%s",defs[i].definition);
          printf("};\n");
        }
      }
    }
    return 0;
  }

  if (!strncmp(format,"info",4)) {
    for (i=0;i<ndefs;i++) {
      if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix)) {
        printf("Name: %s\n",defs[i].name);
        printf("Path: %s\n",defs[i].fullpath);
        printf("Line: %d\n",defs[i].line);
        printf("Members No: %d\n\n",defs[i].members);
      }
    }
    return 0;
  }

  if (!strncmp(format,"members",7)) {
    for (i=0;i<ndefs;i++) {
      if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix)) {

        if (defs[i].isunion)
          printf("Union Name: %s\n",defs[i].name);
        else if (defs[i].isenum)
          printf("Enum Name: %s\n",defs[i].name);
        else 
          printf("Struct Name: %s\n",defs[i].name);
        
        for (j=0;j<defs[i].members;j++) {
          printf("  Member Name: %s\n",defs[i].members_info[j].name);
          printf("  Member Type: %s\n",defs[i].members_info[j].type);
          if (defs[i].members_info[j].array) {
            printf("  Array of %d.\n",defs[i].members_info[j].array);
          }
          if (defs[i].members_info[j].pointer) {
            printf("  It's a pointer of level %d.\n",defs[i].members_info[j].pointer);
          }
          if (defs[i].members_info[j].sub) {
            printf("  It's a sub");
            if (defs[i].members_info[j].sub->isunion) {
              printf("union\n");
            } else if (defs[i].members_info[j].sub->isenum) {
              printf("enum\n");
            } else {
              printf("struct\n");
            }
          }
          if (!strncmp(&format[7],"_data_info",10)) {
            print_memb_data_info(&(defs[i].members_info[j]));
          }
        }
      }
    }
    return 0;
  }

  return 1;
}
