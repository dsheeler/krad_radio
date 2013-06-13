int32_t krad_interweb_client_find_end_of_headers(kr_iws_client_t *client) {

  int i;
  uint8_t *buf;

  buf = client->in->rd_buf;

  for (i = 0; i < client->in->len; i++) {
    if ((buf[i] == '\n') || (buf[i] == '\r')) {
      if (client->hdr_pos != (i - 1)) {
        client->hdr_le = 0;
      }
      client->hdr_pos = i;
      if (buf[i] == '\n') {
        client->hdr_le += 1;
      }
      if (client->hdr_le == 2) {
        buf[client->hdr_pos] = '\0';
        client->hdrs_recvd = 1;
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

int32_t krad_interweb_client_parse_verb(kr_iws_client_t *client) {

  uint8_t *buf;

  buf = client->in->rd_buf;

  if (client->hdr_pos < 8) return -1;
  if (memcmp(buf, "GET ", 4) == 0) {
    client->verb = KR_IWS_GET;
    return 0;
  }
  if (memcmp(buf, "PUT ", 4) == 0) {
    client->verb = KR_IWS_PUT;
    return 0;
  }
  if (memcmp(buf, "HEAD ", 5) == 0) {
    client->verb = KR_IWS_HEAD;
    return 0;
  }
  if (memcmp(buf, "SOURCE ", 7) == 0) {
    client->verb = KR_IWS_SOURCE;
    return 0;
  }
  if (memcmp(buf, "POST ", 5) == 0) {
    client->verb = KR_IWS_POST;
    return 0;
  }
  if (memcmp(buf, "PATCH ", 6) == 0) {
    client->verb = KR_IWS_PATCH;
    return 0;
  }
  if (memcmp(buf, "OPTIONS ", 8) == 0) {
    client->verb = KR_IWS_OPTIONS;
    return 0;
  }

  return -1;
}

int32_t krad_interweb_client_parse_get_request(kr_iws_client_t *client) {

  char *buf;

  buf = (char *)client->in->rd_buf;

  if (strstr(buf, "Upgrade: websocket") != NULL) {
    printk ("Krad Interweb websocket is YEAAY after %zu bytes",
     client->in->len);
    client->type = KR_IWS_WS;
    interweb_get_header (buf, client->get,
      sizeof(client->get), "GET ");
    printk ("WS GET IS %s", client->get);
    interweb_get_header (buf, client->ws.key,
      sizeof(client->ws.key), "Sec-WebSocket-Key: ");
    printk ("KEY IS %s", client->ws.key);
    interweb_get_header (buf, client->ws.proto,
      sizeof(client->ws.proto), "Sec-WebSocket-Protocol: ");
    printk ("PROTO IS %s", client->ws.proto);
    kr_io2_pulled (client->in, client->hdr_pos + 1);
    return 0;
  } else {
    if ((strstr(buf, "GET ") != NULL) &&
        (strstr(buf, " HTTP/1") != NULL)) {
      client->type = KR_IWS_HTTP1;

      interweb_get_header (buf, client->get,
        sizeof(client->get), "GET ");

      printk ("GET IS %s", client->get);

      kr_io2_pulled (client->in, client->hdr_pos);
      return 0;
    }
  }

  return -1;
}

int32_t krad_interweb_client_parse_put_request(kr_iws_client_t *client) {

  char *buf;
  int32_t mount_len;
  char *mount_start;

  mount_start = NULL;
  mount_len = 0;
  buf = (char *)client->in->rd_buf;

  switch (client->verb) {
    case KR_IWS_PUT:
      mount_start = buf + 4;
      mount_len = strcspn(mount_start, " &?\n\r");
      break;
    case KR_IWS_SOURCE:
      mount_start = buf + 7;
      mount_len = strcspn(mount_start, " &?\n\r");
      break;
    default:
      return -1;
  }
  if ((mount_len < 5) || (mount_len > 127)) return -1;
  client->mount[mount_len] = '\0';
  memcpy(client->mount, mount_start, mount_len);
  client->type = KR_IWS_STREAM_IN;
  printk("Source/Put client mount is: %s", client->mount);  

  return 0;
}

int32_t krad_interweb_client_handle_request(kr_iws_client_t *client) {

  int32_t ret;

  if (!client->hdrs_recvd) {
    if (krad_interweb_client_find_end_of_headers(client)) {
      ret = krad_interweb_client_parse_verb(client);
      if (ret < 0) return -1;
      switch (client->verb) {
        case KR_IWS_GET:
          ret = krad_interweb_client_parse_get_request(client);
          return ret;
        case KR_IWS_SOURCE:
          /* Falling Thru */
        case KR_IWS_PUT:
          ret = krad_interweb_client_parse_put_request(client);
          return ret;
        default:
          return -1;
      }
    }
  }

  if ((!client->hdrs_recvd) && (client->in->len >= 4096)) {
    printk("Krad Interweb no header end in sight after %d bytes",
     client->in->len);
    return -1;
  }

  return 0;
}
