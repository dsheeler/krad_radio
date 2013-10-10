#include "json_handler.h"
#define N_TOKENS 128
#define MAX_BUFFER 65536

static uint32_t key_to_value_index(json_handler *jshandler, const char *value, uint32_t len) {

  uint32_t i;
  jsmntok_t *tok;
  uint32_t length;

  for (i=0;i<jshandler->ntokens;i++) {
    tok = &jshandler->tokens[i];
    length = tok->end - tok->start;
    if (tok->type == JSMN_STRING) {
      if (length == len) {
        if (!strncmp(&jshandler->json[tok->start],value,len)) {
          return i+1;
        }
      }
    }
  }

  return 0;
}

static int32_t set_value_from_token(const kr_meta_info_member *member, 
  jsmntok_t *token, char *json) {

  uint32_t length;
  char value_string[256];

  switch (token->type) {
    case JSMN_PRIMITIVE: break;
    case JSMN_STRING: break;
    case JSMN_ARRAY: return 0; /* for now  */
    case JSMN_OBJECT: return 0; /* for now  */
  }

  length = token->end - token->start;
 
  if ( (length < 256) && (length > 0) ) {
    strncpy(value_string,&json[token->start],length);
    value_string[length] = '\0';
  } else {
    return -1;
  }

  switch (member->type) {
    case KR_META_INT32: member->value = atoi(value_string); break;
    case KR_META_UINT32: member->value = atoi(value_string); break;
    case KR_META_UINT64: member->value = atoi(value_string); break;
    case KR_META_FLOAT: member->value = atof(value_string); break;
    case KR_META_CHAR: member->value = value_string[0]; break;
    case KR_META_STRING: strcpy(member->value,value_string); break;
    case KR_META_STRUCT: break;
    case KR_META_ENUM: break;
  }

  return 0;
}

static int32_t mixer_create_handler(json_handler *jshandler, uint32_t type) {

  uint32_t i;
  uint32_t j;
  uint32_t idx;

  for (i=0;i<KR_INFO_TYPES_COUNT;i++) {
    if (kr_meta_info_structs[i]->type == type) {
      for (j=0;j<kr_meta_info_structs[i]->members_count;j++) {

        idx = key_to_value_index(jshandler,kr_meta_info_structs[i]->members[j].name,strlen(kr_meta_info_structs[i]->members[j].name));
        
        if (idx) {
          set_value_from_token(&kr_meta_info_structs[i]->members[j],&jshandler->tokens[idx],jshandler->json);
        }

      }
    /* if (we likey) {
      kr_unit_create(client, &kr_info_meta.info);
    }*/
    }
  }

  return 0;
}

static int32_t create_cmd_handler(json_handler *jshandler) {
  uint32_t idx;
  uint32_t length;
  jsmntok_t *tok;
  char address_str[256];
  kr_address_t address;

  printf("Create command received!\n");

  idx = key_to_value_index(jshandler,"address",7);
  if (!idx) {
    printf("Couldn't find address, aborting\n");
    return -1;
  }

  tok = &jshandler->tokens[idx];
  length = tok->end - tok->start;

  printf("Address found: %.*s\n",length,&jshandler->json[tok->start]);

  if (length < 256) {
    strncpy(address_str,&jshandler->json[tok->start],length);
    address_str[length] = '\0';
    if (kr_string_to_address(address_str,&address) < 0) {
      return -1;
    }
  }

  printf("Address successfully generated\n");

  switch(address.path.unit) {
    case KR_MIXER: { 
      printf("It's mixer\n"); 
      mixer_create_handler(jshandler,KR_MIXER_PATH_INFO); 
      break;
    }
    case KR_COMPOSITOR: printf("It's comp\n"); break;
    case KR_TRANSPONDER: printf("It's xpdr\n"); break;
    case KR_STATION: printf("It's station\n"); break;
  }

  return 0;
}

static int32_t update_cmd_handler(json_handler *jshandler) {
  uint32_t idx;
  uint32_t length;
  jsmntok_t *tok;
  char address_str[256];
  kr_address_t address;
  kr_unit_control_t uc;
  char string[256];

  printf("Update command received!\n");

  idx = key_to_value_index(jshandler,"address",7);
  if (!idx) {
    printf("Couldn't find address, aborting\n");
    return -1;
  }

  tok = &jshandler->tokens[idx];
  length = tok->end - tok->start;

  printf("Address found: %.*s\n",length,&jshandler->json[tok->start]);

  if (length < 256) {
    strncpy(address_str,&jshandler->json[tok->start],length);
    address_str[length] = '\0';
    if (kr_string_to_address(address_str,&address) < 0) {
      return -1;
    }
  }

  printf("Address successfully generated\n");

  memset(&uc,0,sizeof(uc));
  uc.address = address;

  if (kr_address_has_control(&uc.address)) {
    kr_unit_control_data_type_from_address(&uc.address,&uc.data_type);
    switch (uc.data_type) {
      case KR_FLOAT: {
        idx = key_to_value_index(jshandler,"value",5);
        if (!idx) {
          printf("No value found\n");
          return -1;
        }
        tok = &jshandler->tokens[idx];
        length = tok->end - tok->start;
        if (length < 256) {
          strncpy(string,&jshandler->json[tok->start],length);
          string[length] = '\0';
          printf("Found value %s\n",string);
          uc.value.real = atof(string);
        }
        break;
      }
      case KR_INT32: break;
      case KR_STRING: break;
      case KR_CHAR: break;
    }

    idx = key_to_value_index(jshandler,"duration",8);
    if (!idx) {
      printf("No duration found\n");
      return -1;
    }
    tok = &jshandler->tokens[idx];
    length = tok->end - tok->start;
    if (length < 256) {
      strncpy(string,&jshandler->json[tok->start],length);
      string[length] = '\0';
      printf("Found duration %s\n",string);
      uc.duration = atoi(string);
    }
    
    //kr_unit_control_set(jshandler->client,&uc);
    printf("Control updated!\n");
  } else {
    printf("Failed to update\n");
    return -1;
  }

  return 0;
}

static int32_t destroy_cmd_handler(json_handler *jshandler) {
  uint32_t idx;
  uint32_t length;
  jsmntok_t *tok;
  char address_str[256];
  kr_address_t address;

  printf("Destroy command received!\n");

  idx = key_to_value_index(jshandler,"address",7);
  if (!idx) {
    printf("Couldn't find address, aborting\n");
    return -1;
  }

  tok = &jshandler->tokens[idx];
  length = tok->end - tok->start;

  printf("Address found: %.*s\n",length,&jshandler->json[tok->start]);

  if (length < 256) {
    strncpy(address_str,&jshandler->json[tok->start],length);
    address_str[length] = '\0';
    if (kr_string_to_address(address_str,&address) < 0) {
      return -1;
    }
  }

  printf("Address successfully generated\n");

  //kr_unit_destroy(jshandler->client,&address);

  printf("Unit Destroyed\n");

  return 0;
}

static int32_t string_to_cmd_type(const char *cmd, uint32_t len) {

  if (len == 7) {
    if (!strncmp(cmd,"destroy",7)) {
      return CMD_DESTROY;
    }
  }

  if (len == 6) {
    if (!strncmp(cmd,"update",6)) {
      return CMD_UPDATE;
    }
  }

  if (len == 6) {
    if (!strncmp(cmd,"create",6)) {
      return CMD_CREATE;
    }
  }
  
  return -1;
}

static int32_t cmd_handler(json_handler *jshandler) {

  int32_t length;
  int32_t res;
  jsmntok_t *tokens;
  char *buff;

  tokens = jshandler->tokens;
  buff = jshandler->json;
  length = tokens[0].end - tokens[0].start;

  if (length <= 0) {
    printf("Error reading command\n");
    return -1;
  }
  
  if (tokens[0].type != JSMN_STRING) {
    printf("Error reading command\n");
    return -1;
  }

  res = string_to_cmd_type(&buff[tokens[0].start],length);

  switch(res) {
    case CMD_CREATE: {
      if (create_cmd_handler(jshandler) < 0) {
        return -1;
      }
      break; 
    }
    case CMD_UPDATE: {
      if (update_cmd_handler(jshandler) < 0) {
        return -1;
      }
      break; 
    }
    case CMD_DESTROY: {
      if (destroy_cmd_handler(jshandler) < 0) {
        return -1;
      }
      break; 
    }
    case -1: printf("Error reading command\n"); return -1;
  }

  return 0;
}

int32_t main (int32_t argc, char **argv) {

  jsmn_parser parser;
  jsmntok_t tokens[N_TOKENS];
  jsmnerr_t err;
  int32_t nread;
  int32_t res;
  FILE *fd;
  char buff[MAX_BUFFER];
  json_handler jshandler;

  if (argc != 2) {
    printf("Need 1 arg: a path to a json file \n");
    return 1;
  }

  fd = fopen(argv[1],"r");
  if (!fd) {
    fprintf(stderr,"Error: %s\n",strerror(errno));
    return 1;
  }

  printf("Dynamic Structs Initialized\n");

  memset(buff,0,MAX_BUFFER);
  memset(tokens,0,sizeof(jsmntok_t)*N_TOKENS);

  nread = fread(buff,sizeof(char),MAX_BUFFER,fd);

  if (nread <= 0) {
    return 1;
  }

  printf("Parser init\n");
  jsmn_init(&parser);
  printf("Parsing...\n");
  err = jsmn_parse(&parser,buff,tokens,N_TOKENS);

  switch (err) {
    case JSMN_ERROR_NOMEM: printf("Not enough tokens\n"); return 1;
    case JSMN_ERROR_INVAL: printf("Invalid Json\n"); return 1;
    case JSMN_ERROR_PART: printf("Json string is not full\n"); return 1;
    case JSMN_SUCCESS: printf("Json correctly parsed \n"); break;
  }

  jshandler.tokens = tokens;
  jshandler.ntokens = parser.toknext;
  jshandler.json = buff;

  res = cmd_handler(&jshandler);

  if (res < 0) {
    return -1;
  }

  return 0;
}
