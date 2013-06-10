
int32_t krad_interweb_client_find_end_of_headers(kr_iws_client_t *client) {

  int i;
  uint8_t *buf;

  buf = client->in->rd_buf;

  for (i = 0; i < client->in->len; i++) {
    if ((buf[i] == '\n') || (buf[i] == '\r')) {
      if (client->hle_pos != (i - 1)) {
        client->hle = 0;
      }
      client->hle_pos = i;
      if (buf[i] == '\n') {
        client->hle += 1;
      }
      if (client->hle == 2) {
        client->got_headers = 1;
        return 1;
      }
    }
  }
  return 0;
}

int32_t interweb_get_header(char *buf, char *out, uint32_t max, char *header) {

  char *pos;
  int32_t len;
  int32_t hdr_len;
  
  hdr_len = strlen(header);

  pos = strstr(buf, header) + hdr_len;
  if (pos == NULL) {
    return -1;
  }

  len = strcspn(pos, " \n\r?");
  len = MIN(len, max - 1);
  memcpy(out, pos, len);
  out[len] = '\0';

  return 0;
}

int32_t krad_interweb_client_handle_request(kr_iws_client_t *client) {

  char *buf;

  buf = (char *)client->in->rd_buf;

  if (client->got_headers == 0) {
    if (krad_interweb_client_find_end_of_headers(client)) {
      buf[client->hle_pos] = '\0';

      if (strstr(buf, "Upgrade: websocket") != NULL) {
        printk ("Krad Interweb websocket is YEAAY after %zu bytes",
         client->in->len);
        client->type = WS;
        interweb_get_header (buf, client->get,
          sizeof(client->get), "GET ");
        printk ("WS GET IS %s", client->get);
        interweb_get_header (buf, client->ws.key,
          sizeof(client->ws.key), "Sec-WebSocket-Key: ");
        printk ("KEY IS %s", client->ws.key);
        interweb_get_header (buf, client->ws.proto,
          sizeof(client->ws.proto), "Sec-WebSocket-Protocol: ");
        printk ("PROTO IS %s", client->ws.proto);
        kr_io2_pulled (client->in, client->hle_pos + 1);
        return 0;
      } else {
        if ((strstr(buf, "GET ") != NULL) &&
            (strstr(buf, " HTTP/1") != NULL)) {
          client->type = HTTP1;

          interweb_get_header (buf, client->get,
            sizeof(client->get), "GET ");

          printk ("GET IS %s", client->get);

          kr_io2_pulled (client->in, client->hle_pos);
          return 0;
        } else {
          return -1;
        }
      }
    }
  }

  if ((client->got_headers == 0) && (client->in->len >= 4096)) {
    printk ("Krad Interweb no header end in sight after %d bytes",
      client->in->len);
    return -1;
  }

  return 0;
}
