static void krad_interweb_pack_headers(kr_iws_client_t *client,
 char *content_type);

static void krad_interweb_pack_headers(kr_iws_client_t *client,
 char *content_type) {

  int32_t pos;
  char *buffer;
  
  pos = 0;

  buffer = (char *)client->out->buf;
  

  pos += sprintf (buffer + pos, "HTTP/1.0 200 OK\r\n");
  pos += sprintf (buffer + pos, "Status: 200 OK\r\n");
  pos += sprintf (buffer + pos, "Connection: close\r\n");
  pos += sprintf (buffer + pos, "Server: Krad-Radio\r\n");
  pos += sprintf (buffer + pos, "Content-Type: %s; charset=utf-8\r\n",
   content_type);
  pos += sprintf (buffer + pos, "\r\n");

  kr_io2_advance (client->out, pos);
}

static void krad_interweb_pack_404(kr_iws_client_t *client) {

  int32_t pos;
  char *buffer;
  
  pos = 0;

  buffer = (char *)client->out->buf;
  pos += sprintf (buffer + pos, "HTTP/1.1 404 Not Found\r\n");
  pos += sprintf (buffer + pos, "Status: 404 Not Found\r\n");
  pos += sprintf (buffer + pos, "Connection: close\r\n");
  pos += sprintf (buffer + pos, "Server: Krad-Radio\r\n");  
  pos += sprintf (buffer + pos, "Content-Type: text/html; charset=utf-8\r\n");
  pos += sprintf (buffer + pos, "\r\n");
  pos += sprintf (buffer + pos, "404 Not Found");
  
  kr_io2_advance (client->out, pos);  
}
