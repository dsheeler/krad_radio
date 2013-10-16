#include "structs_gathering.h"

static int32_t *clean_string (char *input, char *output) {

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

  return 0;
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
  int file_count;
  struct dirent **dirs;
  struct dirent **files;
  char *rpath;
  int i;
  int j;
  int k;

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
  int nmemb;
  int lenacc;
  char *p1;
  char *p2;
  char *p3;
  int inside_struct = 0;

  fp = fopen(header_path,"r");

  if (fp == NULL) {
    return index;
  }

  linen = 1;
  lenacc = 0;

  while ((read = getline(&line,&len,fp)) != -1) {

    if (inside_struct) {

      /* TODO: handle the case where there is a struct/union definition inside another struct definition */

      p3 = strchr(line,'}');

      if (p3) {
        if (strchr(defs[index].name,'{')) {
          char *end;
          end = strchr(p3,';');
          char sname[end - p3];
          end[0] = '\0';
          clean_string(&p3[1],sname);
          defs[index].name = strdup(sname);
        }
        index++;
        inside_struct = 0;
        lenacc = 0;
      } else {
        if (strchr(line,';')) {
          if ((lenacc+strlen(line)) < MAX_DEF_LENGTH) {
            strcpy(&(defs[index].definition[lenacc]),line);
            lenacc += strlen(line);
            defs[index].members++;
          } else {
            fprintf(stderr,"MAX_DEF_LENGTH exceeded\n");
            exit(1);
          }
        }
      }

    } else {
      p1 = strstr(line,"struct");

      if (p1) {
        p2 = strchr(p1,'{');
        if (p2) {
          char sname[p2-p1];
          clean_string(&p1[6],sname);
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

  return index;
}

int gather_struct_definitions(struct struct_def *defs, char *match, char *text) {
  int i;
  int num;
  int res;
  int ndefs;
  char *headers[MAX_HEADERS];

  num = 0;
  ndefs = 0;
  
  scandir_recursive(text,&num,headers);

  for (i=0;i<num;i++) {
    ndefs = defs_from_header(defs,headers[i],ndefs);
  } 

  for (i=0;i<num;i++) {
    free(headers[i]);
  }

  return num;
}

static int is_prefix (const char *str, const char *prefix) {
  if (!prefix[0]) {
    return 1;
  }
  return !strncmp(str,prefix,strlen(prefix));
}

static int is_suffix (const char *str, const char *suffix) {
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

int print_structs_defs(struct struct_def *defs, 
  int ndefs, char *prefix, char *suffix, char *format) {

  int i;

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
        printf("struct %s {\n",defs[i].name);
        printf("%s",defs[i].definition);
        printf("};\n");
      }
    }
    return 0;
  }

  if (!strncmp(format,"info",4)) {
    for (i=0;i<ndefs;i++) {
      if (is_prefix(defs[i].name,prefix) && is_suffix(defs[i].name,suffix))
        printf("\nName: %s\n",defs[i].name);
        printf("Path: %s\n",defs[i].fullpath);
        printf("Line: %d\n",defs[i].line);
        printf("Members No: %d\n\n",defs[i].members);

    }
    return 0;
  }

  return 1;
}
