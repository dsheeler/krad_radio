char *kr_interweb_server_load_file_or_string(char *input) {

  int fd;
  char *string;
  unsigned int length;
  struct stat file_stat;
  int bytes_read;
  int ret;
  
  fd = 0;
  bytes_read = 0;
  string = NULL;

  if (input == NULL) {
    return NULL;
  }

  if ((strlen(input)) && (input[0] == '/')) {

    fd = open (input, O_RDONLY);
    if (fd < 1) {
      printke("could not open");    
      return NULL;
    }
    fstat (fd, &file_stat);
    length = file_stat.st_size;
    if (length > 1000000) {
      printke("too big");
      close (fd);
      return NULL;
    }

    string = calloc (1, length);        

    while (bytes_read < length) {
    
      ret = read (fd, string + bytes_read, length - bytes_read);

      if (ret < 0) {
        printke("read fail");
        close (fd);
        free (string);
        return NULL;
      }
      bytes_read += ret;
    }
    close (fd);
    return string;
  } else {
    return strdup (input);
  }
}

void kr_interweb_server_setup_html (kr_interweb_t *server) {

  char string[64];
  char *html_template;
  int html_template_len;
  int total_len;
  int len;
  int pos;
  int template_pos;
  
  template_pos = 0;
  pos = 0;
  len = 0;
  total_len = 0;
  
  server->api_js = (char *)lib_krad_web_res_kr_api_js;
  server->api_js_len = lib_krad_web_res_kr_api_js_len;
  server->iface_js  = (char *)lib_krad_web_res_kr_interface_js;
  server->iface_js_len = lib_krad_web_res_kr_interface_js_len;
  server->deviface_js  = (char *)lib_krad_web_res_kr_dev_interface_js;
  server->deviface_js_len = lib_krad_web_res_kr_dev_interface_js_len;

  memset (string, 0, sizeof(string));
  snprintf (string, 7, "%d", server->uberport);
  total_len += strlen(string);
  
  html_template = (char *)lib_krad_web_res_krad_radio_html;
  html_template_len = lib_krad_web_res_krad_radio_html_len - 1;
  total_len += html_template_len - 4;

  server->headcode =
   kr_interweb_server_load_file_or_string(server->headcode_source);
  server->htmlheader =
   kr_interweb_server_load_file_or_string(server->htmlheader_source);
  server->htmlfooter =
   kr_interweb_server_load_file_or_string(server->htmlfooter_source);
  
  if (server->headcode != NULL) {
    total_len += strlen(server->headcode);
  }
  if (server->htmlheader != NULL) {
    total_len += strlen(server->htmlheader);    
  }
  if (server->htmlfooter != NULL) {
    total_len += strlen(server->htmlfooter);    
  }

  server->html_len = total_len;
  server->html = calloc (1, server->html_len + 1);
  
  len = strcspn (html_template, "~");
  strncpy (server->html, html_template, len);
  strcpy (server->html + len, string);
  pos = len + strlen(string);
  template_pos = len + 1;
  
  len = strcspn (html_template + template_pos, "~");
  strncpy (server->html + pos, html_template + template_pos, len);
  template_pos += len + 1;
  pos += len;
  if (server->headcode != NULL) {
    len = strlen(server->headcode);
    strncpy (server->html + pos, server->headcode, len);
    pos += len;
  }
  
  len = strcspn (html_template + template_pos, "~");
  strncpy (server->html + pos, html_template + template_pos, len);
  template_pos += len + 1;
  pos += len;
  if (server->htmlheader != NULL) {
    len = strlen(server->htmlheader);
    strncpy (server->html + pos, server->htmlheader, len);
    pos += len;
  }
  
  len = strcspn (html_template + template_pos, "~");
  strncpy (server->html + pos, html_template + template_pos, len);
  template_pos += len + 1;
  pos += len;
  if (server->htmlfooter != NULL) {
    len = strlen(server->htmlfooter);
    strncpy (server->html + pos, server->htmlfooter, len);
    pos += len;
  }
  
  len = html_template_len - template_pos;
  strncpy (server->html + pos, html_template + template_pos, len);
  template_pos += len;
  pos += len;  
  
  if (template_pos != html_template_len) {
    failfast("html template miscalculation: %d %d", template_pos, html_template_len);
  }

  if (pos != total_len) {
    printke("html miscalculation: %d %d", pos, total_len);
  }

  server->html[total_len] = '\0';

  if (server->headcode != NULL) {
    free (server->headcode);
    server->headcode = NULL;    
  }
  if (server->htmlheader != NULL) {
    free (server->htmlheader);
    server->htmlheader = NULL;    
  }
  if (server->htmlfooter != NULL) {
    free (server->htmlfooter);
    server->htmlfooter = NULL;    
  }    
}
