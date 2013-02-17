#include "krad_http.h"

#include "krad_radio.html.h"
#include "krad_radio.js.h"

#define BUFSIZE 256

krad_http_client_t *krad_http_create_client (krad_http_t *krad_http);
void krad_http_destroy_client (krad_http_client_t *client);
void krad_http_write_headers (krad_http_client_t *client, char *content_type);
void krad_http_write_404 (krad_http_client_t *client);
void *krad_http_client_thread (void *arg);
void *krad_http_server_run (void *arg);
void krad_http_server_setup_html (krad_http_t *krad_http);
char *krad_http_server_load_file_or_string (char *input);

krad_http_client_t *krad_http_create_client (krad_http_t *krad_http) {
  krad_http_client_t *client = calloc (1, sizeof(krad_http_client_t));
  client->krad_http = krad_http;
  return client;
}

void krad_http_destroy_client (krad_http_client_t *client) {
  close (client->sd);
  free (client);
  pthread_exit (0);
}

void krad_http_write_buffer (krad_http_client_t *client, char *buffer, int length) {

  int ret;
  int pos;
  
  pos = 0;
  ret = 0;

  while (pos < length) {
    ret = write (client->sd, buffer + pos, length - pos);
    if (ret < 1) {
      break;
    } else {
      pos += ret;
    }
  }
    
  if (length != pos) {
    printf ("http client write failed...\n");
  }
  //printf ("Wrote %d buffer bytes\n", pos);
  krad_http_destroy_client (client);
}

void krad_http_write_headers (krad_http_client_t *client, char *content_type) {

  int ret;
  int pos;
  char buffer[BUFSIZE];
  
  ret = 0;
  pos = 0;

  pos += sprintf (buffer + pos, "HTTP/1.0 200 OK\r\n");
  pos += sprintf (buffer + pos, "Status: 200 OK\r\n");
  pos += sprintf (buffer + pos, "Connection: close\r\n");
  pos += sprintf (buffer + pos, "Server: Krad-Radio\r\n");
  pos += sprintf (buffer + pos, "Content-Type: %s; charset=utf-8\r\n", content_type);
  pos += sprintf (buffer + pos, "\r\n");

  ret = write (client->sd, buffer, pos);
  
  if (ret != pos) {
    printf ("http client write failed..\n");
    krad_http_destroy_client (client);
  }
  
  //printf ("Wrote %d header bytes\n", pos);
}

void krad_http_write_404 (krad_http_client_t *client) {

  int ret;
  int pos;
  char buffer[BUFSIZE];
  
  ret = 0;
  pos = 0;

  pos += sprintf (buffer + pos, "HTTP/1.1 404 Not Found\r\n");
  pos += sprintf (buffer + pos, "Status: 404 Not Found\r\n");
  pos += sprintf (buffer + pos, "Connection: close\r\n");
  pos += sprintf (buffer + pos, "Server: Krad-Radio\r\n");  
  pos += sprintf (buffer + pos, "Content-Type: text/html; charset=utf-8\r\n");
  pos += sprintf (buffer + pos, "\r\n");
  pos += sprintf (buffer + pos, "404 Not Found");
  
  ret = write (client->sd, buffer, pos);
  
  if (ret != pos) {
    printf ("http client write failed....\n");
  }
  
  krad_http_destroy_client (client);
}

void *krad_http_client_thread (void *arg) {

  krad_http_client_t *client = (krad_http_client_t *)arg;
  
  int ret;
  int getlen;
  int pos;
  char buffer[BUFSIZE * 2];
  char get[BUFSIZE];
  
  getlen = -1;
  ret = 0;
  pos = 0;
  
  ret = read (client->sd, buffer, BUFSIZE * 2);
  
  if (ret < 1) {
    printf ("failed to read browser request");
    krad_http_destroy_client (client);
  }
  
  //printf ("ret is %d\n", ret);

  while (pos < ret) {
    if (strncmp(buffer, "GET /", 5) == 0) {
      getlen = strcspn (buffer + pos + 5, "\r ?");
      memcpy (get, buffer + pos + 5, getlen);
      get[getlen] = '\0';
      break;
    } 
    pos += strcspn (buffer + pos, "\r") + 2;
  }

  if ((getlen > -1) && (getlen < 32)) {
    if (strncmp("krad_radio.js", get, 13) == 0) {
      krad_http_write_headers (client, "text/javascript");
      krad_http_write_buffer (client, client->krad_http->js, client->krad_http->js_len);
    }
    if ((getlen == 0) || ((getlen == 15) && (strncmp("krad_radio.html", get, 15) == 0))) {
      krad_http_write_headers (client, "text/html");
      krad_http_write_buffer (client, client->krad_http->html, client->krad_http->html_len);
    }
    krad_http_write_404 (client);
  }

  krad_http_destroy_client (client);
  return NULL;
}

void *krad_http_server_run (void *arg) {

  krad_http_t *krad_http = (krad_http_t *)arg;

  krad_http_client_t *newclient;
  socklen_t length;
  struct sockaddr_in cli_addr;
  struct pollfd pollfds[2];
  krad_system_set_thread_name ("kr_http");
  int n;

  memset (pollfds, 0, sizeof(pollfds));

  while (!krad_http->shutdown) {

    pollfds[0].fd = krad_controller_get_client_fd (&krad_http->krad_control);
    pollfds[0].events = POLLIN;
    pollfds[1].fd = krad_http->listenfd;
    pollfds[1].events = POLLIN;
  
    n = poll (pollfds, 2, 5000);

    if (n < 0) {
      break;
    }
    if (n > 0) {
      if (pollfds[0].revents) {
        break;
      }
  
      if (pollfds[1].revents == POLLIN) {
        length = sizeof(cli_addr);
        newclient = krad_http_create_client (krad_http);
        if ((newclient->sd = accept(krad_http->listenfd, (struct sockaddr *)&cli_addr, &length)) < 0) {
          close (krad_http->listenfd);
          printf ("krad_http socket error on accept mayb a signal or such\n");
        }

        pthread_create (&newclient->client_thread, NULL, krad_http_client_thread, (void *)newclient);
        pthread_detach (newclient->client_thread);
      }
    }
  }
  
  krad_controller_client_close (&krad_http->krad_control);
  return NULL;
}

char *krad_http_server_load_file_or_string (char *input) {

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

void krad_http_server_setup_html (krad_http_t *krad_http) {

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
  
  memset (string, 0, sizeof(string));
  snprintf (string, 7, "%d", krad_http->websocket_port);
  total_len += strlen(string);
  krad_http->js = (char *)lib_krad_web_res_krad_radio_js;
  krad_http->js_len = lib_krad_web_res_krad_radio_js_len;
  krad_http->js[krad_http->js_len] = '\0';
  
  html_template = (char *)lib_krad_web_res_krad_radio_html;
  html_template_len = lib_krad_web_res_krad_radio_html_len - 1;
  total_len += html_template_len - 4;

  krad_http->headcode = krad_http_server_load_file_or_string (krad_http->headcode_source);
  krad_http->htmlheader = krad_http_server_load_file_or_string (krad_http->htmlheader_source);
  krad_http->htmlfooter = krad_http_server_load_file_or_string (krad_http->htmlfooter_source);
  
  if (krad_http->headcode != NULL) {
    total_len += strlen(krad_http->headcode);
  }
  if (krad_http->htmlheader != NULL) {
    total_len += strlen(krad_http->htmlheader);    
  }
  if (krad_http->htmlfooter != NULL) {
    total_len += strlen(krad_http->htmlfooter);    
  }

  krad_http->html_len = total_len + 1;
  krad_http->html = calloc (1, krad_http->html_len);
  
  len = strcspn (html_template, "~");
  strncpy (krad_http->html, html_template, len);
  strcpy (krad_http->html + len, string);
  pos = len + strlen(string);
  template_pos = len + 1;
  
  len = strcspn (html_template + template_pos, "~");
  strncpy (krad_http->html + pos, html_template + template_pos, len);
  template_pos += len + 1;
  pos += len;
  if (krad_http->headcode != NULL) {
    len = strlen(krad_http->headcode);
    strncpy (krad_http->html + pos, krad_http->headcode, len);
    pos += len;
  }
  
  len = strcspn (html_template + template_pos, "~");
  strncpy (krad_http->html + pos, html_template + template_pos, len);
  template_pos += len + 1;
  pos += len;
  if (krad_http->htmlheader != NULL) {
    len = strlen(krad_http->htmlheader);
    strncpy (krad_http->html + pos, krad_http->htmlheader, len);
    pos += len;
  }
  
  len = strcspn (html_template + template_pos, "~");
  strncpy (krad_http->html + pos, html_template + template_pos, len);
  template_pos += len + 1;
  pos += len;
  if (krad_http->htmlfooter != NULL) {
    len = strlen(krad_http->htmlfooter);
    strncpy (krad_http->html + pos, krad_http->htmlfooter, len);
    pos += len;
  }
  
  len = html_template_len - template_pos;
  strncpy (krad_http->html + pos, html_template + template_pos, len);
  template_pos += len;
  pos += len;  
  
  if (template_pos != html_template_len) {
    failfast("html template miscalculation: %d %d", template_pos, html_template_len);
  }  
  
  if (pos != total_len) {
    printke("html miscalculation: %d %d", pos, total_len);
  }

  krad_http->html[total_len] = '\0';

  if (krad_http->headcode != NULL) {
    free (krad_http->headcode);
    krad_http->headcode = NULL;    
  }
  if (krad_http->htmlheader != NULL) {
    free (krad_http->htmlheader);
    krad_http->htmlheader = NULL;    
  }
  if (krad_http->htmlfooter != NULL) {
    free (krad_http->htmlfooter);
    krad_http->htmlfooter = NULL;    
  }    
}

void krad_http_server_destroy (krad_http_t *krad_http) {

  if (krad_http != NULL) {
    printk ("Krad HTTP shutdown started");  
    krad_http->shutdown = 1;
    if (!krad_controller_shutdown (&krad_http->krad_control, &krad_http->server_thread, 3000)) {
      krad_controller_destroy (&krad_http->krad_control, &krad_http->server_thread);
    }
    close (krad_http->listenfd);
    if (krad_http->html != NULL) {
      free (krad_http->html);
      krad_http->html = NULL;
    }
    free (krad_http);
    printk ("Krad HTTP shutdown complete");
  }
}

krad_http_t *krad_http_server_create (int port, int websocket_port,
                                      char *headcode, char *htmlheader, char *htmlfooter) {
  
  krad_http_t *krad_http = calloc(1, sizeof(krad_http_t));

  struct sockaddr_in serv_addr;
  int on = 1;

  krad_http->port = port;
  krad_http->websocket_port = websocket_port;
  
  krad_http->headcode_source = headcode;
  krad_http->htmlheader_source = htmlheader;
  krad_http->htmlfooter_source = htmlfooter;

  if (krad_http->port < 0 || krad_http->port > 65535) {
    failfast ("krad_http port number error\n");
  }
  
  if (krad_control_init (&krad_http->krad_control)) {
    free (krad_http);
    return NULL;
  }

  printk ("Krad Web Starting Up on port %d", krad_http->port);

  krad_http_server_setup_html (krad_http);

  krad_http->homedir = getenv ("HOME");
   
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(krad_http->port);
   
  /* setup the network socket */
  if ((krad_http->listenfd = socket(AF_INET, SOCK_STREAM,0)) < 0) {
    failfast ("krad_http system call socket error");
  }
  
  if ((setsockopt (krad_http->listenfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))) < 0) {
    close (krad_http->listenfd);
    failfast ("kradweb system call setsockopt error");
  }
  
  if (bind (krad_http->listenfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) <0) {
    close (krad_http->listenfd);
    failfast ("krad_http system call bind error\n");
  }

  if (listen (krad_http->listenfd, SOMAXCONN) <0) {
    close (krad_http->listenfd);
    failfast ("krad_http system call bind error\n");
  }

  pthread_create (&krad_http->server_thread, NULL, krad_http_server_run, (void *)krad_http);

  return krad_http;
}
