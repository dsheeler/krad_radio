#include "cgen_utils.h"

int32_t *clean_string (char *input, char *output) {

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

char *find_info_struct(char *input, char *name) {

  char begsub[] = "struct ";
  char sub2[] = "{";
  char sub3[] = ";";

  char *begp;
  char *pos2;
  char *pos3;
  char res[256];
  char ch;

  begp = strstr(input,begsub);
  if (!begp) {
    return NULL;
  }

  pos2 = strstr(begp,sub2);
  if (!pos2) {
    return NULL;
  }

  pos3 = strstr(begp,sub3);
  if (!pos3 || (pos3 < pos2)) {
    return begp+strlen(begsub);
  }

  ch = pos2[0];
  pos2[0] = '\0';

  clean_string(begp+strlen(begsub),res);

  if (strlen(res) >= 6) {
    if (!strncmp(&res[strlen(res)-5],"_info",5)) {
      strcpy(name,res);
    }
  }

  pos2[0] = ch;

  return begp+strlen(begsub);
}

uint32_t is_dir(const char *path) {
  struct stat buf;
  stat(path, &buf);
  return S_ISDIR(buf.st_mode);
}

int32_t dir_select(const struct direct *entry) {
  if ((strcmp(entry->d_name, ".") == 0) || 
    (strcmp(entry->d_name, "..") == 0) || !is_dir(entry->d_name)) {
    return 0;
  }
  else {
    return 1;
  }
}

uint32_t is_array(char *fname) {

  char token1 = '[';
  char token2 = ']';
  uint32_t num;

  char *tok1p;
  char *tok2p;

  tok1p = strchr(fname,token1);
  tok2p = strchr(fname,token2);

  if (tok1p && tok2p) {
    *tok2p = '\0';
    num = atoi(tok1p+1);
    if (!num) {
      // it's a def! Need to deal with it...
      return 1;
    }
    *tok2p = token2;
    return num;
  } else {
    return 0;
  }
}

int32_t header_files_select(const struct direct *entry) {
  if (is_dir(entry->d_name) || strncmp(&entry->d_name[strlen(entry->d_name)-2],".h",2)) {
    return 0;
  }
  else {
    return 1;
  }
}

static member_type str_to_member_type(char *type) {
  member_type mtype;

  if (!strncmp(type,"int",3)) {
    mtype = KR_DY_INT32;
  } else if (!strncmp(type,"int32_t",7)) {
    mtype = KR_DY_INT32;
  } else if (!strncmp(type,"uint32_t",8)) {
    mtype = KR_DY_UINT32;
  } else if (!strncmp(type,"uint64_t",8)) {
    mtype = KR_DY_UINT64;
  } else if (!strncmp(type,"float",5)) {
    mtype = KR_DY_FLOAT;
  } else if (!strncmp(type,"char",4)) {
    mtype = KR_DY_STRING;
  } else if (!strncmp(type+strlen(type)-5,"_info",5)) {
    mtype = KR_DY_STRUCT;
  } else {
    mtype = KR_DY_ENUM;
  }

  return mtype;
}

dynamic_struct *get_struct_by_name(char *name, 
  dynamic_struct *dystructs, uint32_t n) {

  uint32_t i;

  for (i=0;i<n;i++) {
    if (!strcmp(name,dystructs[i].name)) {
      return &dystructs[i];
    }
  }

  return NULL;
}

static void *set_substructs(dynamic_struct *dystructs, uint32_t n) {

  uint32_t i;
  uint32_t j;
  dynamic_struct *substruct;

  for (i=0;i<n;i++) {
    for (j=0;j<dystructs[i].members;j++) {
      if (dystructs[i].dynamic_members[j].struct_name[0]) {
        substruct = get_struct_by_name(dystructs[i].dynamic_members[j].struct_name,dystructs,n);
        if (substruct) {
          substruct->sub = 1;
        }
      }
    }
  }

  return 0;
}

static void dynamic_members_init(char *line, dynamic_struct *dystruct) {

  char type[32];
  char fname[256];
  char *arraytokp;
  uint32_t num;
  uint32_t nmemb;

  nmemb = dystruct->members;

  clean_string(line,type);
  clean_string(strstr(line,type)+strlen(type),fname);

  if (num = is_array(fname)) {
    printf("Type: %s array ",type);
    arraytokp = strchr(fname,'[');
    *arraytokp = '\0';
    strcpy(dystruct->dynamic_members[nmemb].name,fname);
    dystruct->dynamic_members[nmemb].array = 1;
    dystruct->dynamic_members[nmemb].array_length = num;
    printf("Name: %s Length: %d\n",fname,num);
    *arraytokp = '[';
  } else {
    if (dystruct->dynamic_members[nmemb].type == KR_DY_STRING) {
      dystruct->dynamic_members[nmemb].type = KR_DY_CHAR;
    }
    strcpy(dystruct->dynamic_members[nmemb].name,fname);
  }

  dystruct->dynamic_members[nmemb].type = str_to_member_type(type);
  if (dystruct->dynamic_members[nmemb].type == KR_DY_STRUCT) {
    printf("There's a struct in a struct\n");
    strcpy(dystruct->dynamic_members[nmemb].struct_name,type);
  } else if (dystruct->dynamic_members[nmemb].type == KR_DY_ENUM) {
    printf("This must be an enum\n");
  }
  printf("Type: %s ",type);
  printf("Name: %s \n",fname);


  dystruct->members++;

  return;
}

static void dynamic_struct_init(char *input, dynamic_struct *dystruct) {
  char *tokenp;
  char *pos;
  char *end_tokenp;
  char *field_line;

  pos = strchr(input,'{');
  end_tokenp = strstr(pos,"};");
  
  while ((tokenp = strchr(pos,';') ) && (tokenp < end_tokenp) ) {
    *tokenp = '\0';
    if (*pos == '{') {
      field_line = pos+1;
    }
    else {
      field_line = pos;
    }
    dynamic_members_init(field_line,dystruct);
    *tokenp = ';';
    pos = tokenp+1;
  }

  return;
}

int32_t parse_and_init(char *basepath,dynamic_struct *dystructs) {

  char dpath[256];
  char sname[32];
  char cwd[256];
  uint32_t dcount;
  uint32_t fcount;
  uint32_t i;
  uint32_t j;
  uint32_t k;
  struct dirent **dirs;
  struct dirent **files;
  FILE *fp;
  char buffer[MAX_BUFFER];
  int32_t nread;
  char *input_left;

  if (getcwd(cwd,sizeof(cwd)) < 0) {
    return -1;
  }

  if (chdir(basepath) < 0) {
    return -1;
  }

  k = 0;
  dcount = scandir(basepath, &dirs, dir_select, alphasort);

  if (dcount < 0) {
    return -1;
  }

  for (i=0;i<dcount;i++) {
    strcpy(dpath,basepath);
    strcat(dpath,dirs[i]->d_name);
    if (chdir(dpath) < 0) {
      return -1;
    }
    
    fcount = scandir(dpath, &files, header_files_select, alphasort);

    if (fcount < 0) {
      return -1;
    }

    for (j=0;j<fcount;j++) {
      fp = fopen(files[j]->d_name, "r");
      if (fp) {
        nread = fread(buffer,sizeof(char),MAX_BUFFER,fp);
        if (nread >= 0) {
          buffer[nread-1] = '\0';
          input_left = buffer;
          do {
            memset(sname,0,32);
            input_left = find_info_struct(input_left,sname);
            if (sname[0]) {
              printf("\nIn file %s/%s\nstruct %s:\n\n",dpath,files[j]->d_name,sname);
              strcpy(dystructs[k].name,sname);
              dynamic_struct_init(input_left,&dystructs[k]);
              k++;
            } 
          }
          while (input_left);
        }
        fclose(fp);
      } else {
        return -1;
      }
    }

    if (chdir("..")) {
      return -1;
    }
  }

  set_substructs(dystructs,k);

  if (chdir(cwd)) {
    return -1;
  }

  return k;
}
